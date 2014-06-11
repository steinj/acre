/*
 * Copyright 2014 Joshua Stein.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "config.h"

#include <iostream>
#include <wordexp.h>


#define CONFIG_EETHRESH "early_exit_threshold"
#define CONFIG_OUTPUT "output_file"
#define CONFIG_IGNORE "ignore"

AcreConfig::AcreConfig() {
}

AcreConfig::~AcreConfig() {
  if (out != &(std::cout)) {
    dynamic_cast<std::ofstream*>(out)->close();
    free(out);
  }
}

/**
 * Expands environment variables within strings.
 * @param inStr string to be expanded
 * @return string with all environment variables expanded
 */
std::string expandString(std::string inStr) {
  std::string outStr;
  wordexp_t p;
  char **w;

  wordexp(inStr.c_str(), &p, 0);
  outStr = p.we_wordv[0];
  wordfree(&p);
  
  return outStr;
}

/**
 * Reads and parses the config file. 
 * Defaults to ${HOME}/.acre.config if path is empty.
 * @param path path to config file
 */
void AcreConfig::initConfig(std::string path) {
  if (path.empty()) {
    path = expandString("${HOME}/.acre.config");
  }
  try {
    config.readFile(path.c_str());
    if (config.exists(CONFIG_IGNORE)) {
      libconfig::Setting &igList = config.lookup(CONFIG_IGNORE);
      for (int i = 0; i < igList.getLength(); i++)
        ignore.insert(expandString(igList[i]));
    }
  }
  // Silently ignore I/O errors
  catch(libconfig::FileIOException &e) {}
}

/**
 * Retrieve output stream.
 * @return pointer to output stream
 */
std::ostream* AcreConfig::getOut() {
  return out;
}

/**
 * Opens output stream.
 * Defaults to std::cout if fname is empty.
 * @param fname name of file to output violations to.
 */
void AcreConfig::initOut(std::string fname) {
  out = &(std::cout);
  std::string outFile = fname;

  // fname empty -> fallback to config
  if (fname.empty() && config.exists(CONFIG_OUTPUT))
    config.lookupValue(CONFIG_OUTPUT, outFile);

  // Redirect to file if possible
  if (!outFile.empty()) {
    try {
      std::ofstream *outFileStream = new std::ofstream();
      outFileStream->open(outFile.c_str(), std::ofstream::out | std::ofstream::app);
      out = outFileStream;
    }
    catch(std::exception &e) {}
  }
}

/**
 * Return the early exit threshold.
 * @return early exit threshold, 20 if not set
 */
int AcreConfig::getEarlyExitThreshold() {
  int retVal = 20;
  config.lookupValue(CONFIG_EETHRESH, retVal);
  return retVal;
}

/**
 * Changes the early exit threshold to the specified value.
 * @param value value to set threshold to
 */
void AcreConfig::setEarlyExitThreshold(int value) {

  libconfig::Setting *eethresh;
  if (config.exists(CONFIG_EETHRESH)) {
    eethresh = &config.lookup(CONFIG_EETHRESH);
  }
  else {
    libconfig::Setting &root = config.getRoot();
    eethresh = &root.add(CONFIG_EETHRESH, libconfig::Setting::TypeInt);
  }
  *eethresh = value;
}

/**
 * Determine if a file should be ignored (includes the file being in an ignored directory).
 * @param fname file to check
 * @return true if file should be ignored, false otherwise
 */
bool AcreConfig::isIgnoreFile(std::string fname) {
  
  // File explicitly ignored
  if (ignore.count(fname) != 0)
    return true;

  // Check if file is in ignored directory
  int pos = fname.length();
  while (pos >= 0) {
    int idx = fname.rfind("/", pos);
    if (std::string::npos == idx)
      return false;

    if (ignore.count(fname.substr(0, idx+1)))
      return true;

    pos = idx - 1;
  }
  return false;
}

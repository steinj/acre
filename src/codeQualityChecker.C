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


#include "rose.h"
#include <iostream>
#include <fstream>
#include <exception>

#include "config.h"
#include "acrePragmaTraversal.h"
#include "returnElseTraversal.h"
#include "structClassKeywordTraversal.h"
#include "staticConstructorTraversal.h"
#include "loopConditionTraversal.h"
#include "earlyExitCandidateTraversal.h"

#define PREFIX "-acre:"

/**
 * Annotates AST nodes based on the acre specific pragmas
 * @param project the project to check
 */
void pragmaProcessing(SgProject *project) {
  AcrePragmaTraversal apt;
  apt.traverseInputFiles(project, postorder);
}

/**
 * Checks a file for control flow violations involving Return/Continue/etc before else statements.
 * @param file the file to check
 * @param os output stream that violations are directed to
 */
void returnElseCheck(SgFile *file, std::ostream *os) {
  ReturnElseInheritedAttr inheritedAttribute;
  ReturnElseTraversal reTraversal(os);
  reTraversal.traverseWithinFile(file, inheritedAttribute);
}

/**
 * Checks a file for structs that have protected/private member variables/functions.
 * @param file the file to check
 * @param os output stream that violations are directed to
 */
void structClassCheck(SgFile *file, std::ostream *os) {
  StructClassKeywordTraversal sckTraversal(os);
  sckTraversal.traverseWithinFile(file);
}

/**
 * Checks for declaration of variables whose type has a static constructor.
 * @param file the file to check
 * @param os output stream that violations are directed to
 */
void staticConstructorCheck(SgFile *file, std::ostream *os) {
  StaticConstructorTraversal scTraversal(os);
  scTraversal.traverseWithinFile(file, postorder);
}

/**
 * Checks for loops with calls to the end() member function.
 * @param file the file to check
 * @param os output stream that violations are directed to
 */
void loopConditionCheck(SgFile *file, std::ostream *os) {
  LoopConditionTraversal lcTraversal(os);
  lcTraversal.traverseWithinFile(file, postorder);
}

/**
 * Checks for if statements that are candidates for exiting early from a function or loop.
 * @param file the file to check
 * @param os output stream that violations are directed to
 * @param earlyExitThreshold the minimum number of statements necessary to report an if statement
 */
void earlyExitCandidateCheck(SgFile *file, std::ostream *os, int earlyExitThreshold) {
  EarlyExitCandidateTraversal eecTraversal(os, earlyExitThreshold);
  eecTraversal.traverseWithinFile(file, 0);
}

/**
 * Checks for "#include <iostream>"
 * @param file the file to check
 * @param os output stream that violations are directed to
 */
void iostreamIncludeCheck(SgFile *file, std::ostream *os) {
  ROSEAttributesListContainerPtr rAttrList = file->get_preprocessorDirectivesAndCommentsList();
  for (auto it = rAttrList->getList().begin(); it != rAttrList->getList().end(); ++it) {
    for (auto jt = it->second->getList().begin(); jt != it->second->getList().end(); ++jt) {

      // Only check #includes
      if ((*jt)->getTypeOfDirective() == PreprocessingInfo::DirectiveType::CpreprocessorIncludeDeclaration) {
        if ((*jt)->getString().find("iostream") != std::string::npos) {
          Sg_File_Info *fInfo = (*jt)->get_file_info();
          *os << "#include <iostream> Violation: " << fInfo->get_filename() << " @ " << (*jt)->getLineNumber() << "\n";
        }
      }
    }
  }
}

/**
 * Outputs the usage of acre.
 * @param name the name used to call acre
 */
void printUsage(char *name) {
  std::cerr << "Usage: "<< name << " [OPTION]... " << "[FILENAME]..." << "\n";
  std::cerr << "ACRE Options:\n";
  std::cerr << "\t" << "-acre:config_file <filename>        (Default: ${HOME}/.acre.config" << "\n";
  std::cerr << "\t" << "-acre:output_file <filename>        (Default: cout)" << "\n";
  std::cerr << "\t" << "-acre:early_exit_threshold <value>  (Default: 20)"   << "\n"; 
}

int main ( int argc, char* argv[] ) {
  if (argc <= 1) {
    printUsage(argv[0]);
    return 1;
  }

  /**
   * Process ACRE CLI arguments
   */
  AcreConfig config;

  // Get arguments
  Rose_STL_Container<std::string> l = CommandlineProcessing::generateArgListFromArgcArgv(argc, argv);

  // Load config file
  std::string configFile;
  CommandlineProcessing::isOptionWithParameter(l, PREFIX, "config_file", configFile, true);
  config.initConfig(configFile);

  // Redirect output to a file
  std::string outFile;
  CommandlineProcessing::isOptionWithParameter(l, PREFIX, "output_file", outFile, true);
  config.initOut(outFile);

  // Get early exit threshold -- used for detecting excessive if statement length
  int eethresh;
  if (CommandlineProcessing::isOptionWithParameter(l, PREFIX, "early_exit_threshold", eethresh, true)) {
    config.setEarlyExitThreshold(eethresh);
  }

  /**
   * Process source files/AST
   */
  // Build AST 
  SgProject *project = frontend(l);
  ROSE_ASSERT (project != NULL);

  // Preprocessing -- pragma
  pragmaProcessing(project);

  // Process project file-by-file
  SgFileList *fileList = project->get_fileList_ptr();
  for (auto file = fileList->get_listOfFiles().begin(); file != fileList->get_listOfFiles().end(); ++file) {

    // Skip specified files
    if (config.isIgnoreFile((*file)->getFileName()))
      continue;

    // Check for violations in the coding standards
    iostreamIncludeCheck(*file, config.getOut());
    returnElseCheck(*file, config.getOut());
    structClassCheck(*file, config.getOut());
    staticConstructorCheck(*file, config.getOut());
    loopConditionCheck(*file, config.getOut());
    earlyExitCandidateCheck(*file, config.getOut(), config.getEarlyExitThreshold());
  }

  // Clean-up

  return backend(project);
}

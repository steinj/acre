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


#pragma once

#include <fstream>
#include <set>
#include <libconfig.h++>

class AcreConfig {
  public:
    AcreConfig(); 
    ~AcreConfig();

    void initConfig(std::string path);

    std::ostream* getOut();
    void initOut(std::string fname);

    int getEarlyExitThreshold();
    void setEarlyExitThreshold(int value);

    bool isIgnoreFile(std::string fname);
  private:
    libconfig::Config config;
    std::ostream *out;
    std::set<std::string> ignore;
};

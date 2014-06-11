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

#include "rose.h"
#include <iostream>

enum acrePragma_enum {
  pragma_acre_ignore,
  pragma_acre_ignore_recursive,
  pragma_acre_none
};

/*
---------- grammar begin -----------
% 'string' means literal string to be matched
% | means alternation

acre_pragma = '#pragma' hc_simple_part

acre_simple_part = 'acre' 'ignore'

---------------- grammar end -----------
*/

class ACRE_PragmaAttribute : public AstAttribute {
  public:
    SgNode *node;
    enum acrePragma_enum pragma_type;

    ACRE_PragmaAttribute(SgNode *n, acrePragma_enum p_type) : node(n), pragma_type(p_type) { }

    virtual std::string toString() {
      std::string result;
      result += "#pragma ";
      switch (pragma_type) {
        case pragma_acre_ignore:
          result += "acre ignore";
          break;
        case pragma_acre_ignore_recursive:
          result += "acre ignore recursive";
          break;
        default:
          std::cerr << "Error. ACRE_PragmaAttribute::toString(), illegal pragma type.\n";
          assert(false);
      }
      return result;
    }
};

class AcrePragmaTraversal : public AstSimpleProcessing {
  public:
    virtual void visit(SgNode* n);
};

/**
 * Determines if the node is annotated with the ignore pragma
 * @param n node to check
 * @return true if the node should be ignored, false otherwise
 */
bool isAcreIgnore(SgNode *n);

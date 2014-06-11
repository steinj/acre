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

class ReturnElseInheritedAttr {
  public:
    SgStatement *elseBranch;
    ReturnElseInheritedAttr(): depth(0), elseBranch(NULL) {};
    ReturnElseInheritedAttr(int x, SgStatement *branch): depth(x), elseBranch(branch) {};
    ReturnElseInheritedAttr(const ReturnElseInheritedAttr &X): depth(X.depth), elseBranch(X.elseBranch) {};
    int getDepth() { return depth; }
  private:
    int depth;
};

class ReturnElseTraversal : public AstTopDownProcessing<ReturnElseInheritedAttr> {
  public:
    ReturnElseTraversal(std::ostream *os) : out(os) {};
    virtual ReturnElseInheritedAttr evaluateInheritedAttribute(SgNode* n, ReturnElseInheritedAttr inheritedAttribute);
  private:
    std::ostream *out;
};

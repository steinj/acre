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

class EarlyExitInheritedAttr {
  public:
    EarlyExitInheritedAttr() : depth(0) {};
    EarlyExitInheritedAttr(int d) : depth(d) {};
    EarlyExitInheritedAttr(const EarlyExitInheritedAttr &X) : depth(X.depth) {};
    int  getDepth() { return depth; }
    void incDepth() { depth++; }
  private:
    int depth; // IF stmt depth
};

class EarlyExitSynAttr {
  public:
    EarlyExitSynAttr() : numStmts(0) {};
    EarlyExitSynAttr(int n) : numStmts(n) {};
    EarlyExitSynAttr operator() (EarlyExitSynAttr a, EarlyExitSynAttr b) {
      return EarlyExitSynAttr(a.numStmts + b.numStmts);
    }
    void incStmts() { numStmts++; }
    int getNumStmts() { return numStmts; }
  private:
    int numStmts;
};

class EarlyExitCandidateTraversal : public AstTopDownBottomUpProcessing<EarlyExitInheritedAttr, EarlyExitSynAttr> {
  public:
    EarlyExitCandidateTraversal(std::ostream *os, int t) : out(os), threshold(t) {};
    virtual EarlyExitInheritedAttr evaluateInheritedAttribute(SgNode* n, EarlyExitInheritedAttr inheritedAttribute);
    virtual EarlyExitSynAttr evaluateSynthesizedAttribute ( 
        SgNode* n, EarlyExitInheritedAttr inheritedAttribute, 
        SynthesizedAttributesList childAttributes );
  private:
    std::ostream *out;
    int threshold;
};

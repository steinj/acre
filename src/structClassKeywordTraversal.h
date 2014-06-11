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

class MemberVisibilitySynAttr {
  public:
    MemberVisibilitySynAttr(): numPrivateProtected(0), numPublic(0) {};
    MemberVisibilitySynAttr operator() (MemberVisibilitySynAttr a, MemberVisibilitySynAttr b) {
      MemberVisibilitySynAttr sum;
      sum.numPrivateProtected = a.numPrivateProtected + b.numPrivateProtected;
      sum.numPublic = a.numPublic + b.numPublic;
      return sum;
    }
    int getNumPrivateProtected() { return numPrivateProtected; }
    int getNumPublic() { return numPublic; }
    void incPrivateProtected() { numPrivateProtected++; }
    void incPublic() { numPublic++; }
    void reset() {
      numPrivateProtected = 0;
      numPublic = 0;
    }
  private:
    int numPrivateProtected;
    int numPublic;
};

class StructClassKeywordTraversal : public AstBottomUpProcessing<MemberVisibilitySynAttr> {
  public:
    StructClassKeywordTraversal(std::ostream *os) : out(os) {};
    virtual MemberVisibilitySynAttr evaluateSynthesizedAttribute(SgNode* n, SynthesizedAttributesList childAttrs);
  private:
    SgClassDeclaration::class_types getClassType(SgClassDeclaration *n);
    std::ostream *out;
};

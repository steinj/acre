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


#include "loopConditionTraversal.h"

#include "acrePragmaTraversal.h"
#include <numeric>
#include <functional>

class EndFunctionTraversal : public AstBottomUpProcessing<int> {
  public:
    virtual int evaluateSynthesizedAttribute(SgNode* n, SynthesizedAttributesList childAttrs);
};

void LoopConditionTraversal::visit(SgNode *n) {
  SgNode *cond = NULL;

  // Get condition from loops
  if (SgWhileStmt *whileLoop = isSgWhileStmt(n)) {
    cond = whileLoop->get_condition();
  }
  else if (SgForStatement *forLoop = isSgForStatement(n)) {
    cond = forLoop->get_test();
  }
  else if (SgDoWhileStmt *doWhileLoop = isSgDoWhileStmt(n)) {
    cond = doWhileLoop->get_condition();
  }

  // Loop found: check condition for compliance
  Sg_File_Info *fInfo = n->get_file_info();
  if (NULL != cond && !isAcreIgnore(n) && !fInfo->isCompilerGenerated()) {
    EndFunctionTraversal eft;
    int numEnd = eft.traverse(cond);
    if (numEnd > 0) {
      *out << "Evaluation of end() in Loop Condition Violation: " << fInfo->get_filename() << " @ " << \
        fInfo->get_line() << " [x" << numEnd << "]\n";
    }
  }
}

int EndFunctionTraversal::evaluateSynthesizedAttribute(SgNode* n, SynthesizedAttributesList childAttrs) {
  int endCalls = std::accumulate(childAttrs.begin(), childAttrs.end(), 0, std::plus<int>());

  // Member function found: determine if the name is end(), i.e. iterator call
  if (SgMemberFunctionRefExp *funcRef = isSgMemberFunctionRefExp(n)) {
    SgMemberFunctionSymbol *funcSym = funcRef->get_symbol();
    SgName funcName = funcSym->get_name();

    if (funcName.getString().compare("end") == 0) {
      endCalls++;
    }
  }

  return endCalls;
}

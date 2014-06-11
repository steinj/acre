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


#include "earlyExitCandidateTraversal.h"

#include "acrePragmaTraversal.h"
#include <numeric>

EarlyExitInheritedAttr EarlyExitCandidateTraversal::evaluateInheritedAttribute(SgNode* n, EarlyExitInheritedAttr inheritedAttribute) {

  if (isSgIfStmt(n)) {
    inheritedAttribute.incDepth();
  }

  return inheritedAttribute;
}


EarlyExitSynAttr EarlyExitCandidateTraversal::evaluateSynthesizedAttribute ( 
        SgNode* n, EarlyExitInheritedAttr inheritedAttribute, 
        SynthesizedAttributesList childAttrs ) {

  EarlyExitSynAttr lines;

  // Ignore conditional statement of IF stmt
  int attrOffset = 0;
  SgIfStmt *ifStmt = isSgIfStmt(n);
  if (ifStmt) 
    attrOffset++;

  lines = std::accumulate(childAttrs.begin() + attrOffset, childAttrs.end(), lines, lines);

  // Check for violation -- only consider outer-most IF stmt
  Sg_File_Info *fInfo = n->get_file_info();
  if (ifStmt && NULL == ifStmt->get_false_body() && lines.getNumStmts() >= threshold && 1 == inheritedAttribute.getDepth() 
      && !isAcreIgnore(ifStmt) && !fInfo->isCompilerGenerated()) {
    *out << "Early Exit Candidate: " << fInfo->get_filename() << " @ " << \
      fInfo->get_line() << " [" << lines.getNumStmts() << " statements]\n";
  }

  // Increment stmts -- ignore scopes
  if (isSgStatement(n) && !isSgScopeStatement(n)) {
    lines.incStmts();
  }

  return lines;
}

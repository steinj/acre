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


#include "returnElseTraversal.h"

#include "acrePragmaTraversal.h"

ReturnElseInheritedAttr ReturnElseTraversal::evaluateInheritedAttribute(SgNode* n, ReturnElseInheritedAttr inheritedAttribute) {
  int incDepth = 0;
  SgStatement *elseBranch = NULL;

  // Inside else branch: Reduce depth
  if (n == inheritedAttribute.elseBranch) {
    incDepth--;
  }

  switch(n->variantT()) {
    // Possible control flow violations
    case V_SgReturnStmt:
    case V_SgBreakStmt:
    case V_SgContinueStmt:
    case V_SgGotoStatement:
      if (inheritedAttribute.getDepth() > 0 && !isAcreIgnore(n)) {
        Sg_File_Info *fInfo = n->get_file_info();
        *out << "Return-Else Violation: " << fInfo->get_filename() << " @ " << fInfo->get_line() << " [x" << inheritedAttribute.getDepth() << "]\n";
      }
      break;

      // Check for precurser to control flow violation
    case V_SgIfStmt:
      if (elseBranch = isSgIfStmt(n)->get_false_body()) {
        incDepth++;
      }
      break;

    default:
      {} 
      // Do nothing
  }

  return ReturnElseInheritedAttr(inheritedAttribute.getDepth() + incDepth, elseBranch);
}

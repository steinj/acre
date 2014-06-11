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


#include "structClassKeywordTraversal.h"

#include "acrePragmaTraversal.h"
#include <numeric>
#include <algorithm>

MemberVisibilitySynAttr StructClassKeywordTraversal::evaluateSynthesizedAttribute(SgNode* n, SynthesizedAttributesList childAttrs) {
  MemberVisibilitySynAttr visibility;

  visibility = std::accumulate(childAttrs.begin(), childAttrs.end(), visibility, visibility);

  // Check for visibility violation in the class declaration
  if (SgClassDeclaration *cDec = isSgClassDeclaration(n)) {
    Sg_File_Info *fInfo = n->get_file_info();

    if (!fInfo->isCompilerGenerated() && !cDec->isForward() && !isAcreIgnore(n)) {

      SgClassDeclaration::class_types type = getClassType(cDec);

      // Structs should have ONLY PUBLIC members
      if (SgClassDeclaration::e_struct == type && 0 < visibility.getNumPrivateProtected()) {
        *out << "Struct Member Visibility Violation: " << fInfo->get_filename() << " @ " << \
          fInfo->get_line() << " [" << visibility.getNumPrivateProtected() << " Private/Protected members]\n";
      }
#ifdef DEBUG
      // Class should have private members (encapsulation)
      else if (SgClassDeclaration::e_class == type && 0 == visibility.getNumPrivateProtected()) {
        *out << "Class Member Visibility Violation: " << fInfo->get_filename() << " @ " << \
          fInfo->get_line() << " [All members public]\n";
      }
#endif
    }

    // Prevent cascading errors by removing information from nested defintions
    visibility.reset();
  }

  // Update member visibility information
  if (SgDeclarationStatement *decStat = isSgDeclarationStatement(n)) {
    const SgAccessModifier accessMod = decStat->get_declarationModifier().get_accessModifier();
    if (accessMod.isPublic()) {
      visibility.incPublic();
    }
    else if (accessMod.isPrivate() || accessMod.isProtected()) {
      visibility.incPrivateProtected();
    }
  }

  return visibility;
}

SgClassDeclaration::class_types StructClassKeywordTraversal::getClassType(SgClassDeclaration *n) {
  SgClassDeclaration::class_types type = n->get_class_type();

  // (Reference)StackOverflow (for removing spaces): http://stackoverflow.com/questions/83439/remove-spaces-from-stdstring-in-c
  // Remove all spaces from unparsed string and node name
  std::string cSource = n->unparseToString();
  cSource.erase(remove_if(cSource.begin(), cSource.end(), isspace), cSource.end());
  std::string cName = n->get_name().getString();
  cName.erase(remove_if(cName.begin(), cName.end(), isspace), cName.end());

  // Remove all template information from the class name, if it exists
  int templatePos = cName.find("<");
  if (-1 != templatePos)
    cName.erase(cName.begin() + templatePos, cName.end());

  // Find name and determine class type
  int namePos = cSource.find(cName);
  if (5 <= namePos) {
    cSource = cSource.substr(namePos-5,5);

    // Class type = struct, make the correction
    if (n->e_class == type && cSource.compare(0, 5, "truct") == 0) {
      type = n->e_struct;
    }
  }

  return type;
}

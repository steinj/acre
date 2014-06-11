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


#include "staticConstructorTraversal.h"

#include "acrePragmaTraversal.h"

void StaticConstructorTraversal::visit(SgNode *n) {

  // Get declared variables
  SgInitializedName *vName = isSgInitializedName(n);

  if (vName && !isAcreIgnore(vName->get_declaration())) {
    Sg_File_Info *fInfo = vName->get_file_info();
    SgScopeStatement *scope = vName->get_scope();
    
    // Find global variables (variables in namespaces count, e.g. std)
    if (!fInfo->isCompilerGenerated() && (isSgGlobal(scope) || isSgNamespaceDefinitionStatement(scope))) {

      // Walk typedefs until reach pointer to base type  
      SgTypedefType *tdType = isSgTypedefType(vName->get_type());
      while (tdType && isSgTypedefType(tdType->get_base_type())) 
        tdType = isSgTypedefType(tdType->get_base_type());
      
      // Determine if type is a class (i.e. type with a constructor)
      SgClassType *cType = isSgClassType(vName->get_type());
      if (tdType)
        cType = isSgClassType(tdType->get_base_type());
      
      // Output location of globals with a static constructor
      if (cType) {
        *out << "Static Constructor Violation: " << fInfo->get_filename() << " @ " << fInfo->get_line() << "\n";
      }
    }
  }
}

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


#include "acrePragmaTraversal.h"

AstAttribute* parse_ACRE_Pragma(SgPragmaDeclaration *pragmaDecl) {
  AstAttribute *result = NULL;
  assert(pragmaDecl != NULL);
  assert(pragmaDecl->get_pragma() != NULL);

  std::string pragmaString = pragmaDecl->get_pragma()->get_pragma();

  // save variables before changes
  const char *old_char = AstFromString::c_char;
  SgNode *old_node = AstFromString::c_sgnode;

  AstFromString::c_sgnode = SageInterface::getNextStatement(pragmaDecl);
  assert(AstFromString::c_sgnode != NULL);

  AstFromString::c_char = pragmaString.c_str();

  // Parse Pragma
  if (AstFromString::afs_match_substr("acre")) {

    if (AstFromString::afs_match_substr("ignore")) {

      if (AstFromString::afs_match_substr("recursive")) {
        result = new ACRE_PragmaAttribute(AstFromString::c_sgnode, pragma_acre_ignore_recursive);
        assert(result != NULL);
      }
      else {
        result = new ACRE_PragmaAttribute(AstFromString::c_sgnode, pragma_acre_ignore);
        assert(result != NULL);
      }
    }
  }

  // undo side effects
  AstFromString::c_char = old_char;
  AstFromString::c_sgnode = old_node;
  return result;
}

/**
 * Determine if the given node defines a scope and if so return it.
 * @param n node to check
 * @return pointer to a scope, null if not a scope
 */
SgScopeStatement* retrieveScope(SgNode *n) {
  SgStatement *stmt = isSgStatement(n);
  switch (stmt->variantT()) {
    case V_SgNamespaceDeclarationStatement:
      stmt = isSgNamespaceDeclarationStatement(stmt)->get_definition();
      break;
    case V_SgClassDeclaration:
      stmt = isSgClassDeclaration(stmt)->get_definition();
      break;
    case V_SgFunctionDeclaration:
    case V_SgMemberFunctionDeclaration:
      stmt = isSgFunctionDeclaration(stmt)->get_definition();
      break;
    default:
      NULL;
  }
  return isSgScopeStatement(stmt);
}

/**
 * Recursively annotate all nodes in the scope with the given attribute
 * @param scope pointer to the scope to annotate
 * @param attr attribute to set for all statements within the scope
 */
void ignoreNestedScope(SgScopeStatement *scope, AstAttribute *attr) {
  SgStatementPtrList sList = scope->generateStatementList();
  for (auto it = sList.begin(); it != sList.end(); ++it) {
    if (SgScopeStatement *inner = retrieveScope(*it)) {
      ignoreNestedScope(inner, attr);
    }
    (*it)->addNewAttribute("acre_pragma", attr);
  }
}

void AcrePragmaTraversal::visit(SgNode *n) {
  if (SgPragmaDeclaration *pragmaDecl = isSgPragmaDeclaration(n)) {
    AstAttribute *result = parse_ACRE_Pragma(pragmaDecl);
    SgStatement *stmt = SageInterface::getNextStatement(pragmaDecl);

    // Valid acre pragma and following statement
    if (result && stmt) {
      ACRE_PragmaAttribute *acrePragma = dynamic_cast<ACRE_PragmaAttribute*>(result);
      
      // recursive pragmas handled require recursive annotation
      if (pragma_acre_ignore_recursive == acrePragma->pragma_type) {
        SgScopeStatement *scope = retrieveScope(stmt);
        assert(scope != NULL);
        ignoreNestedScope(scope, result);
      }

      // Always annotate the following statement
      stmt->addNewAttribute("acre_pragma", result);
    }
  }
}

bool isAcreIgnore(SgNode *n) {

  if (!n->attributeExists("acre_pragma"))
    return false;

  ACRE_PragmaAttribute *attr = dynamic_cast<ACRE_PragmaAttribute*>(n->getAttribute("acre_pragma"));
  assert(attr != NULL);

  bool isIgnore = false;
  switch (attr->pragma_type) {
    case pragma_acre_ignore:
    case pragma_acre_ignore_recursive:
      isIgnore = true;
      break;
    default:
      isIgnore = false;
  }
  return isIgnore;
}

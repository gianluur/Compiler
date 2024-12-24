#include "identifier.h"
#include "ASTNode.h"

Identifier::Identifier(const Token& token): 
  ASTNode(ASTNodeType::IDENTIFIER), m_str(token.lexemes) {}

void Identifier::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Identifier: " << m_str << '\n';
}

string Identifier::toString() const {
  return m_str;
}

const ASTNodeType Identifier::getIdentifierType(const Identifier* identifier) const {
  const string name = identifier->m_str;
  if (!scope->isDeclared(name))
    error("Identifier: " + name + " is not declared");
  return scope->find(name).type;
}
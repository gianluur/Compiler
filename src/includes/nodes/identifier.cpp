#include "identifier.h"
#include "parameter.h"
#include "variable.h"
#include "function.h"
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
  if (!Scope::getInstance()->isDeclared(name))
    error("Identifier: " + name + " is not declared");

  const Symbol& symbol = Scope::getInstance()->find(name);
  if (symbol.type == ASTNodeType::VARIABLE){
    const Variable* variable = std::get<const Variable*>(symbol.symbol);
    return variable->getType();
  }
  else if (symbol.type == ASTNodeType::FUNCTION){
    const Function* function = std::get<const Function*>(symbol.symbol);
    return function->getType();
  }
  else if (symbol.type == ASTNodeType::PARAMETER){
    const Parameter* parameter = std::get<const Parameter*>(symbol.symbol);
    return parameter->getType();
  }
  else 
    error("Unexpected error when obtaining identifier type\n");
}
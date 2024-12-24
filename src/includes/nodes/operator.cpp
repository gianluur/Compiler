#include "operator.h"
#include "ASTNode.h"

Operator::Operator(const Token& token): 
  ASTNode(ASTNodeType::OPERATOR), op(token.type), m_str(token.lexemes) {}

void Operator::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Operator: " << m_str << '\n';
}

TokenType Operator::getOperator() const {
  return op;
}

string Operator::toString() const {
  return m_str;
}
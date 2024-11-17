#include "operator.h"

Operator::Operator(const Token& token) : op(token.type), m_str(token.lexemes) {}

void Operator::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Operator: " << m_str << '\n';
}

TokenType Operator::getOperator() const {
  return op;
}
#include "unary_operator.h"

UnaryOperator::UnaryOperator(unique_ptr<Operator> op, unique_ptr<ASTNode> right)
  : m_op(std::move(op)), m_right(std::move(right)) {}

void UnaryOperator::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Unary Operator{\n";
  m_op->print(indentation_level + 2);
  m_right->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

TokenType UnaryOperator::getOperator() const {
  return m_op->getOperator();
}
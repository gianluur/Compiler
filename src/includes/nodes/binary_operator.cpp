#include "binary_operator.h"
#include "ASTNode.h"

BinaryOperator::BinaryOperator(unique_ptr<ASTNode> left, unique_ptr<Operator> op, unique_ptr<ASTNode> right): 
  m_left(std::move(left)), m_op(std::move(op)), m_right(std::move(right)) {}

void BinaryOperator::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Binary Operator{\n";
  m_left->print(indentation_level + 2);
  m_op->print(indentation_level + 2);
  m_right->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

ASTNode* BinaryOperator::getLeft() const {
  return m_left.get();
}

TokenType BinaryOperator::getOperator() const {
  return m_op->getOperator();
}

ASTNode* BinaryOperator::getRight() const {
  return m_right.get();
}
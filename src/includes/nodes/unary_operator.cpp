#include "ASTNode.h"
#include "expression.h"
#include "unary_operator.h"

UnaryOperator::UnaryOperator(unique_ptr<Operator> op, unique_ptr<ASTNode> right): 
  ASTNode(ASTNodeType::UNARY_OPERATOR), m_op(std::move(op)), m_right(std::move(right)) {}

void UnaryOperator::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Unary Operator{\n";
  m_op->print(indentation_level + 2);
  m_right->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

TokenType UnaryOperator::getOperator() const {
  return m_op->getOperator();
}

const ASTNode* UnaryOperator::getRight() const {
  return m_right.get();
}

ASTNodeType UnaryOperator::analyzeUnaryOperator(const UnaryOperator* unaryOperator) const {
  return Expression::analyzeExpression(unaryOperator->getRight());
}
#include "binary_operator.h"
#include "ASTNode.h"
#include "expression.h"

#include "../../backend/codegen.h"

BinaryOperator::BinaryOperator(unique_ptr<ASTNode> left, unique_ptr<Operator> op, unique_ptr<ASTNode> right): 
  ASTNode(ASTNodeType::BINARY_OPERATOR), m_left(std::move(left)), m_op(std::move(op)), m_right(std::move(right)) {}

void BinaryOperator::accept(Codegen* generator) const {
  generator->visit(this);
}

void BinaryOperator::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Binary Operator{\n";
  m_left->print(indentation_level + 2);
  m_op->print(indentation_level + 2);
  m_right->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

const ASTNode* BinaryOperator::getLeft() const {
  return m_left.get();
}

enum TokenType BinaryOperator::getOperator() const {
  return m_op->getOperator();
}

const ASTNode* BinaryOperator::getRight() const {
  return m_right.get();
}

ASTNodeType BinaryOperator::analyzeBinaryOperator(const BinaryOperator* binaryOperator) const {
  binaryOperator->print(0);

  const ASTNodeType leftOperand = Expression::analyzeExpression(binaryOperator->getLeft());
  const ASTNodeType rightOperand = Expression::analyzeExpression(binaryOperator->getRight());

  if (!Type::AreEquals(leftOperand, rightOperand)) 
    error("In expressions the left and right operand in a binary operator must have the same type: " 
      + std::to_string(static_cast<int>(leftOperand)) + " " + std::to_string(static_cast<int>(rightOperand)));
  
  if (binaryOperator->m_op->isComparisonOperator())
    return ASTNodeType::BOOL;

  return leftOperand; 
}

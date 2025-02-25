#include "ASTNode.h"
#include "expression.h"
#include "unary_operator.h"

#include "../../backend/codegen.h"

UnaryOperator::UnaryOperator(unique_ptr<Operator> op, unique_ptr<ASTNode> right): 
  ASTNode(ASTNodeType::UNARY_OPERATOR), m_op(std::move(op)), m_right(std::move(right)) {}

void UnaryOperator::accept(Codegen* generator) const {
  generator->visit(this);
}

void UnaryOperator::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Unary Operator{\n";
  m_op->print(indentation_level + 2);
  m_right->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

enum TokenType UnaryOperator::getOperator() const {
  return m_op->getOperator();
}

const ASTNode* UnaryOperator::getRight() const {
  return m_right.get();
}

ASTNodeType UnaryOperator::analyzeUnaryOperator(const UnaryOperator* unaryOperator) const {
  const ASTNodeType type = Expression::analyzeExpression(unaryOperator->getRight());
  const enum TokenType op = unaryOperator->getOperator();

  if (op == TokenType::CARET){
    if ((type >= ASTNodeType::INT && type <= ASTNodeType::FLOAT64_PTR) && !(static_cast<int>(type) % 2 == 0))
      error("Unary operator '^' can only be used to dereference pointers");
    return static_cast<ASTNodeType>(static_cast<int>(type) - 1);
  }
  else if (op == TokenType::AMPERSAND){
    return static_cast<ASTNodeType>(static_cast<int>(type) + 1);
  }
  else
    return type;
}
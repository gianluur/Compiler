#pragma once

#include "ASTNode.h"
#include "operator.h"
#include "expression.h"

class UnaryOperator : public ASTNode {
public:
  UnaryOperator(unique_ptr<Operator> op, unique_ptr<ASTNode> right);
  void print(int indentation_level = 0) const override;
  TokenType getOperator() const;
  const ASTNode* getRight() const;
  const ASTNodeType getType() const;
  ASTNodeType analyzeUnaryOperator(const UnaryOperator* unaryOperator) const;

private:
  unique_ptr<Operator> m_op;
  unique_ptr<ASTNode> m_right;
};
#pragma once

#include "ASTNode.h"
#include "operator.h"

class UnaryOperator : public ASTNode {
public:
  UnaryOperator(unique_ptr<Operator> op, unique_ptr<ASTNode> right);
  void print(int indentation_level = 0) const override;
  TokenType getOperator() const;

private:
  unique_ptr<Operator> m_op;
  unique_ptr<ASTNode> m_right;
};
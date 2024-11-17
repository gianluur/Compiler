#pragma once

#include "ASTNode.h"
#include "operator.h"

class BinaryOperator : public ASTNode {
public:
  BinaryOperator(unique_ptr<ASTNode> left, unique_ptr<Operator> op, unique_ptr<ASTNode> right);
  void print(int indentation_level = 0) const override;
  ASTNode* getLeft() const;
  TokenType getOperator() const;
  ASTNode* getRight() const;

private:
  unique_ptr<ASTNode> m_left;
  unique_ptr<Operator> m_op;
  unique_ptr<ASTNode> m_right;
};
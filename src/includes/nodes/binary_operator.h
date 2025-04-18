#pragma once

#include "ASTNode.h"
#include "operator.h"

class BinaryOperator : public ASTNode {
public:
  BinaryOperator(unique_ptr<ASTNode> left, unique_ptr<Operator> op, unique_ptr<ASTNode> right);

  void accept(Codegen* generator) const override;
  void print(int indentation_level = 0) const override;

  const ASTNode* getLeft() const;
  enum TokenType getOperator() const;
  const ASTNode* getRight() const;
  
  ASTNodeType analyzeBinaryOperator(const BinaryOperator* binaryOperator) const;

private:
  unique_ptr<ASTNode> m_left;
  unique_ptr<Operator> m_op;
  unique_ptr<ASTNode> m_right;
};
#pragma once
#include "ASTNode.h"
#include "literal.h"

class Expression : public ASTNode {
public:
  Expression(unique_ptr<ASTNode> start);
  Expression();
  void print(int indentation_level = 0) const override;
  ASTNode* getExpression() const;

private:
  unique_ptr<ASTNode> m_start;
};
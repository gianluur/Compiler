#pragma once

#include "ASTNode.h"
#include "expression.h"

class Return: public ASTNode {
public:
  Return(unique_ptr<Expression> expression, const enum TokenType scope);

  void accept(Codegen* generator) const override;
  void print(int indentation_level = 0) const override;

  Expression* getExpression() const;
  enum TokenType getScope() const;

private:
  unique_ptr<Expression> m_expression;
  const enum TokenType m_scope;
};
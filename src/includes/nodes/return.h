#pragma once

#include "ASTNode.h"
#include "expression.h"
#include "type.h"

class Return: public ASTNode {
public:
  Return(const unique_ptr<Type>& type, unique_ptr<Expression> expression, const enum TokenType scope);

  void accept(Codegen* generator) const override;
  void print(int indentation_level = 0) const override;

  Expression* getValue() const;
  enum TokenType getScope() const;
  void analyzeReturn() const;

private:
  const unique_ptr<Type>& type;
  unique_ptr<Expression> m_expression;
  const enum TokenType m_scope;
};
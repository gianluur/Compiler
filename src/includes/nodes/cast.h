#pragma once

#include "ASTNode.h"
#include "expression.h"
#include "type.h"

class Cast: public ASTNode {
public:
  Cast(unique_ptr<Type> type, unique_ptr<Expression> expression);

  void print(int indentation_level = 0) const override;
  ASTNode* getExpression() const;
  TokenType getType() const;

private:
  unique_ptr<Type> m_type;
  unique_ptr<Expression> m_expression;
};
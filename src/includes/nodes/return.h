#include "ASTNode.h"
#include "expression.h"

class Return: public ASTNode {
public:
  Return(unique_ptr<Expression> expression, const TokenType scope);
  void print(int indentation_level = 0) const override;
  Expression* getExpression() const;
  TokenType getScope() const;

private:
  unique_ptr<Expression> m_expression;
  const TokenType m_scope;
};
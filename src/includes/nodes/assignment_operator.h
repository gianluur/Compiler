#pragma once

#include "ASTNode.h"
#include "identifier.h"
#include "operator.h"
#include "expression.h"

class AssignmentOperator : public ASTNode {
public:
  AssignmentOperator(unique_ptr<Identifier> identifier, unique_ptr<Operator> op, unique_ptr<Expression> value);
  void print(int indentation_level = 0) const override;
  TokenType getOperator() const;
  Identifier* getIdentifier() const;
  ASTNode* getExpression() const;

private:  
  unique_ptr<Identifier> m_identifier;
  unique_ptr<Operator> m_op;
  unique_ptr<Expression> m_value;
};
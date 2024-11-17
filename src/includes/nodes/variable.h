#pragma once

#include "ASTNode.h"
#include "identifier.h"
#include "expression.h"
#include "type.h"

class Variable: public ASTNode {
public:
  Variable(const Token& keyword, unique_ptr<Type> type, unique_ptr<Identifier> identifier);
  Variable(const Token& keyword, unique_ptr<Type> type, unique_ptr<Identifier> identifier, unique_ptr<Expression> value);

  void print(int indentation_level = 0) const override;

  string getKeyword() const;
  TokenType getType() const;
  ASTNode* getValue() const;

private:
  const Token& m_keyword;
  unique_ptr<Type> m_type;
  unique_ptr<Identifier> m_identifier;
  unique_ptr<Expression> m_value;
};
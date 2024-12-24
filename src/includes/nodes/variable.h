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
  ASTNodeType getType() const;
  ASTNode* getValue() const;
  bool isPointer() const;

private:
  const Token& m_keyword;
  unique_ptr<Type> m_type;
  unique_ptr<Identifier> m_identifier;
  unique_ptr<Expression> m_value;
};
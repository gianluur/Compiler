#pragma once

#include "ASTNode.h"
#include "expression.h"
#include "identifier.h"
#include "list_initializer.h"
#include "type.h"

class Identifier;

class Variable: public ASTNode {
public:
  using ValueVariant = variant<string, unique_ptr<Expression>, unique_ptr<ListInitializer>>;

  Variable(const Token& keyword, unique_ptr<Type> type, unique_ptr<Identifier> identifier, const bool isMember);
  Variable(const Token& keyword, unique_ptr<Type> type, unique_ptr<Identifier> identifier, ValueVariant value, const bool isMember);

  void print(int indentation_level = 0) const override;

  string getKeyword() const;
  ASTNodeType getType() const;
  ASTNode* getValue() const;
  bool isPointer() const;
  void analyzeVariable() const;
  string getIdentifier() const;
  string getTypeToString() const;
  
private:
  const Token& m_keyword;
  unique_ptr<Type> m_type;
  unique_ptr<Identifier> m_identifier;
  ValueVariant m_value;
  const bool m_isMember;
};
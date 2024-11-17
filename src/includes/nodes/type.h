#pragma once
#include "ASTNode.h"

class Type: public ASTNode {
public:
  Type(const Token& token);
  void print(int indentation_level = 0) const override;
  TokenType getType() const;
  bool isInteger() const;
  bool isFloat() const;
  bool isBool() const;
  bool isChar() const;
  bool isNull() const;
  string toString() const;

private:
  const TokenType m_type;
  const string m_str;
};
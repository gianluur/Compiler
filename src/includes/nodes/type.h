#pragma once
#include "ASTNode.h"

class Type: public ASTNode {
public:
  Type(const Token& token, const bool isPointer);
  void print(int indentation_level = 0) const override;
  ASTNodeType getType() const;
  bool isInteger() const;
  bool isFloat() const;
  bool isBool() const;
  bool isChar() const;
  bool isNull() const;
  bool isPointer() const;
  string toString() const;
  ASTNodeType TokenTypeToASTNodeType(const TokenType type) const;

private:
  const TokenType m_type;
  const string m_str;
  const bool m_isPointer;
};
#pragma once
#include "ASTNode.h"

class Literal : public ASTNode {
public:
  Literal(const Token& token);
  Literal();

  void print(int indentation_level = 0) const override;

  TokenType getType() const;

  bool isInteger() const;
  bool isFloat() const;
  bool isBool() const;
  bool isChar() const;
  bool isNull() const;

  string toString() const;
  ASTNodeType LiteralTokenToASTNodeTypeLiteral(const TokenType tokenType);

private:
  const TokenType m_type;
  const string m_str;
};
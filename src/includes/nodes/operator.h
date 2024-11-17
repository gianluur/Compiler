#pragma once
#include "ASTNode.h"

class Operator : public ASTNode {
public:
  Operator(const Token& token);
  void print(int indentation_level = 0) const override;
  TokenType getOperator() const;

private:
  const TokenType op;
  const string m_str;
};
#pragma once
#include "ASTNode.h"

class Operator : public ASTNode {
public:
  Operator(const Token& token);

  void accept(Codegen* generator) const override;
  void print(int indentation_level = 0) const override;

  enum TokenType getOperator() const;
  string toString() const;
  bool isComparisonOperator() const;

private:
  const enum TokenType op;
  const string m_str;
};
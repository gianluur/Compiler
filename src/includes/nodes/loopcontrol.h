#pragma once
#include "ASTNode.h"

class LoopControl: public ASTNode {
public:
  LoopControl(const Token& token, const enum TokenType scope);

  void accept(Codegen* generator) const override;
  void print(int indentation_level = 0) const override;

  string getKeyword() const;
  enum TokenType getScope() const;

private:
  const string m_str;
  const enum TokenType m_scope;
};
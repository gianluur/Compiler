#pragma once
#include "ASTNode.h"

class Identifier : public ASTNode {
public:
  Identifier(const Token& token);
  void print(int indentation_level = 0) const override;
  string toString() const;

private:
  const string m_str;
};
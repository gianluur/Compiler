#pragma once
#include "ASTNode.h"

class Identifier : public ASTNode {
public:
  Identifier(const Token& token);
  
  void accept(Codegen* generator) const override;
  void print(int indentation_level = 0) const override;

  string toString() const;
  ASTNodeType getIdentifierType(const Identifier* identifier) const;

private:
  const string m_str;
};
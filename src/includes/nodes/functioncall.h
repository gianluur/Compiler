#pragma once
#include "ASTNode.h"
#include "identifier.h"
#include "expression.h"

class FunctionCall: public ASTNode {
public:
  FunctionCall(unique_ptr<Identifier> identifier, vector<unique_ptr<Expression>> arguments);
  void print(int indentation_level = 0) const override;
private:
  unique_ptr<Identifier> m_identifier;
  vector<unique_ptr<Expression>> m_arguments;
};
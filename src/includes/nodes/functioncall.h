#pragma once
#include "ASTNode.h"
#include "identifier.h"
#include "expression.h"

class Expression;

class FunctionCall: public ASTNode {
public:
  FunctionCall(unique_ptr<Identifier> identifier, vector<unique_ptr<Expression>> arguments);
  void print(int indentation_level = 0) const override;
  const ASTNodeType getFunctionCallType(const FunctionCall* identifier) const;
private:
  unique_ptr<Identifier> m_identifier;
  vector<unique_ptr<Expression>> m_arguments;
};
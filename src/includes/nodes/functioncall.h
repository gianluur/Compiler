#pragma once
#include "ASTNode.h"
#include "identifier.h"
#include "expression.h"

class Expression;

class FunctionCall: public ASTNode {
public:
  FunctionCall(unique_ptr<Identifier> identifier, vector<unique_ptr<Expression>> arguments, const bool isInsideExpression);

  void accept(Codegen* generator) const override;
  void print(int indentation_level = 0) const override;

  ASTNodeType analyzeFunctionCall(const FunctionCall* functionCall) const;
  vector<Expression*> getArguments() const;
  
private:
  unique_ptr<Identifier> m_identifier;
  vector<unique_ptr<Expression>> m_arguments;
  const bool m_isInsideExpression;
};
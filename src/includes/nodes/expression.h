#pragma once
#include "ASTNode.h"
#include "literal.h"
#include "binary_operator.h"
#include "unary_operator.h"
#include "functioncall.h"
#include "dot_operator.h"
#include "identifier.h"
#include "cast.h"

class Expression : public ASTNode {
public:
  Expression(unique_ptr<ASTNode> start, const bool isCondition);
  Expression();
  void print(int indentation_level = 0) const override;
  ASTNode* getExpression() const;
  ASTNodeType getType() const;
  static ASTNodeType analyzeExpression(const ASTNode* expression);
  static void analyzeCondition(const ASTNode* condition);

private:
  unique_ptr<ASTNode> m_start;
  bool m_isCondition;
  ASTNodeType m_type;
};
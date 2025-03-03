#pragma once
#include "ASTNode.h"

class Expression : public ASTNode {
public:
  Expression(unique_ptr<ASTNode> start, const bool isCondition);
  Expression();

  void accept(Codegen* generator) const override;
  void print(int indentation_level = 0) const override;

  ASTNode* getASTNode() const;
  ASTNodeType getType() const;
  static ASTNodeType analyzeExpression(const ASTNode* expression);
  static void analyzeCondition(const ASTNode* condition);

private:
  unique_ptr<ASTNode> m_start;
  bool m_isCondition;
  ASTNodeType m_type;
};
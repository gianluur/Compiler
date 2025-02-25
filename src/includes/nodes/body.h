#pragma once

#include "ASTNode.h"

class Body: public ASTNode {
public:
  Body(vector<unique_ptr<ASTNode>> statements);
  
  void accept(Codegen* generator) const override;
  void print(int indentation_level = 0) const override;

  vector<ASTNode*> getStatements() const;

private:
  vector<unique_ptr<ASTNode>> m_statements;
};
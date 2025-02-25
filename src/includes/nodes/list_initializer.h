#pragma once

#include <memory>
#include <vector> 

#include "ASTNode.h"
#include "expression.h"

class ListInitializer: public ASTNode {
public:
  ListInitializer(vector<unique_ptr<Expression>> list);

  void accept(Codegen* generator) const override;
  void print(int indentation_level = 0) const override;

  vector<Expression*> getList() const;
private:
  vector<unique_ptr<Expression>> m_list;
};
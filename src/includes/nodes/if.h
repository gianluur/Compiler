#pragma once

#include "expression.h"
#include "body.h"
#include "else.h"


class Else;

class If: public ASTNode {
public:
  If(unique_ptr<Expression> condition, unique_ptr<Body> body, vector<unique_ptr<Else>> elses);

  void accept(Codegen* generator) const override;
  void print(int indetation_level = 0) const override;
  
  ASTNode* getCondition() const;
  vector<ASTNode*> getBody() const;
  vector<Else*> getElses() const;

private:
  unique_ptr<Expression> m_condition;
  unique_ptr<Body> m_body;
  vector<unique_ptr<Else>> m_elses;
};
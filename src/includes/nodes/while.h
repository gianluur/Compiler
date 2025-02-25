#pragma once

#include "ASTNode.h"
#include "expression.h"
#include "body.h"

class While: public ASTNode {
public:
  While(unique_ptr<Expression> condition, unique_ptr<Body> body);
  
  void accept(Codegen* generator) const override;
  void print(int indetation_level = 0) const override;

  ASTNode* getCondition() const;
  vector<ASTNode*> getBody() const;

private:
  unique_ptr<Expression> m_condition;
  unique_ptr<Body> m_body;
};
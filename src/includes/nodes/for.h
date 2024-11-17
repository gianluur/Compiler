#pragma once
#include "ASTNode.h"
#include "body.h"
#include "variable.h"
#include "expression.h"
#include "assignment_operator.h"

class For: public ASTNode {
public:
  For(unique_ptr<Variable> initialization, unique_ptr<Expression> condition, unique_ptr<AssignmentOperator> update, unique_ptr<Body> body);
  
  void print(int indentation_level = 0) const;
  
  Variable* getInitialization() const;
  
  AssignmentOperator* getUpdate() const;
  
  Expression* getCondition() const;
  
  vector<ASTNode*> getBody() const;

private:
  unique_ptr<Variable> m_initialization;
  unique_ptr<Expression> m_condition;
  unique_ptr<AssignmentOperator> m_update;
  unique_ptr<Body> m_body;
};
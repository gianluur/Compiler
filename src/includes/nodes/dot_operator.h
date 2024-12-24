#pragma once

#include "ASTNode.h"
#include "assignment_operator.h"
#include "expression.h"
#include "identifier.h"

class AssignmentOperator;

class DotOperator: public ASTNode {
public:
  DotOperator(unique_ptr<Identifier> identifier, unique_ptr<AssignmentOperator> assigment);
  DotOperator(unique_ptr<Identifier> identifier, unique_ptr<Identifier> member);
  void print(int indentation_level = 0) const override;

private:
  unique_ptr<Identifier> m_identifier;
  unique_ptr<AssignmentOperator> m_assigment;
  unique_ptr<Identifier> m_member;
};



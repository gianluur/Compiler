#include "assignment_operator.h"

AssignmentOperator::AssignmentOperator(unique_ptr<Identifier> identifier, unique_ptr<Operator> op, unique_ptr<Expression> value):
  m_identifier(std::move(identifier)), m_op(std::move(op)), m_value(std::move(value)) {}

void AssignmentOperator::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "Assignment Operator {\n";
  m_identifier->print(indentation_level + 2);
  m_op->print(indentation_level + 2);
  m_value->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

Identifier* AssignmentOperator::getIdentifier() const {
  return m_identifier.get();
}

TokenType AssignmentOperator::getOperator() const {
  return m_op->getOperator();
}

ASTNode* AssignmentOperator::getExpression() const {
  return m_value->getExpression();
}
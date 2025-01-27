#include "assignment_operator.h"
#include "ASTNode.h"
#include "expression.h"
#include "function.h"
#include "operator.h"
#include "variable.h"

AssignmentOperator::AssignmentOperator(unique_ptr<Identifier> identifier, unique_ptr<Operator> op, unique_ptr<Expression> value, const bool isDotOperator):
  ASTNode(ASTNodeType::ASSIGNMENT_OPERATOR), m_identifier(std::move(identifier)), m_op(std::move(op)), m_value(std::move(value)), m_isDotOperator(isDotOperator) {
    analyzeAssignmentOperator();
  }

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

string AssignmentOperator::getOperatorToString() const {
  return m_op->toString();
}

ASTNode* AssignmentOperator::getExpression() const {
  return m_value->getExpression();
}

void AssignmentOperator::analyzeAssignmentOperator() const {
  if (m_isDotOperator)
    return;

  const Symbol& symbol = Scope::getInstance()->find(m_identifier->toString());
  ASTNodeType identifierType;
  
  if (symbol.type == ASTNodeType::VARIABLE)
    identifierType = std::get<const Variable*>(symbol.symbol)->getType();
  else if (symbol.type == ASTNodeType::FUNCTION)
    identifierType = std::get<const Function*>(symbol.symbol)->getType();
  else
    error("Unexpected error while analizing assignment operator"); 

  if (!Type::AreEquals(identifierType, m_value->getType()))
    error("In assignment operator the type and the value type doesn't match");
}
#include "assignment_operator.h"
#include "ASTNode.h"
#include "expression.h"
#include "function.h"
#include "operator.h"
#include "parameter.h"
#include "variable.h"
#include <string>

AssignmentOperator::AssignmentOperator(unique_ptr<Identifier> identifier, unique_ptr<Operator> op, unique_ptr<Expression> value, const bool isDotOperator, const bool isDereference):
  ASTNode(ASTNodeType::ASSIGNMENT_OPERATOR), m_identifier(std::move(identifier)), m_op(std::move(op)), m_value(std::move(value)), m_isDotOperator(isDotOperator), m_isDereference(isDereference) {
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
  ASTNodeType identifierType, valueType = m_value->getType();
  
  if (symbol.type == ASTNodeType::VARIABLE)
    identifierType = std::get<const Variable*>(symbol.symbol)->getType();
  else if (symbol.type == ASTNodeType::FUNCTION)
    identifierType = std::get<const Function*>(symbol.symbol)->getType();
  else if (symbol.type == ASTNodeType::PARAMETER)
    identifierType = std::get<const Parameter*>(symbol.symbol)->getType();
  else
    error("Unexpected error while analizing assignment operator"); 

  if (m_isDereference && ((identifierType >= ASTNodeType::INT && identifierType <= ASTNodeType::FLOAT64_PTR) && identifierType % 2 == 0))
    identifierType = static_cast<ASTNodeType>(static_cast<int>(identifierType) - 1);

  if (!Type::AreEquals(identifierType, valueType))
    error("In assignment operator the type and the value type doesn't match: " + to_string(identifierType) + " " + to_string(valueType));
}
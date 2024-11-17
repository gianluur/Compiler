#include "cast.h"

Cast::Cast(unique_ptr<Type> type, unique_ptr<Expression> expression): 
  m_type(std::move(type)), m_expression(std::move(expression)) {}

void Cast::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Cast {\n";
  m_type->print(indentation_level + 2);
  m_expression->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

ASTNode* Cast::getExpression() const {
  return m_expression->getExpression();
}

TokenType Cast::getType() const {
  return m_type->getType();
}
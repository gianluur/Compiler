#include "return.h"
#include "ASTNode.h"
#include "expression.h"
#include <iomanip>

Return::Return(unique_ptr<Expression> expression, const TokenType scope):
  ASTNode(ASTNodeType::RETURN), m_expression(std::move(expression)), m_scope(scope) {}

void Return::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "Return {\n";
  m_expression->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n"; 
}

Expression* Return::getExpression() const {
  return m_expression.get();
}

TokenType Return::getScope() const {
  return m_scope;
}
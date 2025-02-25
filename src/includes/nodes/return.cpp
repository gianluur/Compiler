#include "return.h"
#include "ASTNode.h"
#include "expression.h"

#include "../../backend/codegen.h"

Return::Return(unique_ptr<Expression> expression, const enum TokenType scope):
  ASTNode(ASTNodeType::RETURN), m_expression(std::move(expression)), m_scope(scope) {}

void Return::accept(Codegen* generator) const {
  generator->visit(this);
}

void Return::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "Return {\n";
  m_expression->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n"; 
}

Expression* Return::getExpression() const {
  return m_expression.get();
}

enum TokenType Return::getScope() const {
  return m_scope;
}
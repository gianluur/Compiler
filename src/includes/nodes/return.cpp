#include "return.h"
#include "ASTNode.h"
#include "expression.h"

#include "../../backend/codegen.h"
#include <nodes/type.h>
#include <token.hpp>

Return::Return(const unique_ptr<Type>& type, unique_ptr<Expression> expression, const enum TokenType scope):
  ASTNode(ASTNodeType::RETURN), type(type), m_expression(std::move(expression)), m_scope(scope) {
    analyzeReturn();
  }

void Return::accept(Codegen* generator) const {
  generator->visit(this);
}

void Return::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "Return {\n";
  m_expression->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n"; 
}

Expression* Return::getValue() const {
  return m_expression.get();
}

enum TokenType Return::getScope() const {
  return m_scope;
}

void Return::analyzeReturn() const {
  if (m_scope != TokenType::FUNC){
    error("Return statement can't be outside a function scope");
    return;
  }

  if (!Type::AreEquals(type->getType(), m_expression->getType()))
    error("Return statement value type and return type doesn't match");
  
}
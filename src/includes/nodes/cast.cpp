#include "cast.h"
#include "ASTNode.h"

#include "../../backend/codegen.h"

Cast::Cast(unique_ptr<Type> type, unique_ptr<Expression> expression): 
  ASTNode(ASTNodeType::CAST), m_type(std::move(type)), m_expression(std::move(expression)) {}

void Cast::accept(Codegen* generator) const {
  generator->visit(this);
}

void Cast::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Cast {\n";
  m_type->print(indentation_level + 2);
  m_expression->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

ASTNodeType Cast::getType() const {
  return m_type->getNodeType();
}

ASTNode* Cast::getExpression() const {
  return m_expression->getASTNode();
}

ASTNodeType Cast::analyzeCast() const {
  return m_type->getNodeType();
}
#include "expression.h"
#include "literal.h"

Expression::Expression(unique_ptr<ASTNode> start): 
  m_start(std::move(start)) {}
Expression::Expression(): 
  m_start(make_unique<Literal>()) {}

void Expression::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Expression {\n";
  m_start->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

ASTNode* Expression::getExpression() const {
  return m_start.get();
}
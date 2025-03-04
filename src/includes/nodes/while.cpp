#include "while.h"

#include "../../backend/codegen.h"

While::While(unique_ptr<Expression> condition, unique_ptr<Body> body):
  ASTNode(ASTNodeType::WHILE), m_condition(std::move(condition)), m_body(std::move(body)) {}

void While::accept(Codegen* generator) const {
  generator->visit(this);
}

void While::print(int indetation_level) const {
  cout << '\n' << setw(indetation_level) << " " << "While Statement{\n";
  m_condition->print(indetation_level + 2);
  m_body->print(indetation_level + 2);
  cout << setw(indetation_level) << " " << "}\n";
}

ASTNode* While::getCondition() const {
  return m_condition->getASTNode();
}

vector<ASTNode*> While::getBody() const {
  return m_body->getStatements();
}
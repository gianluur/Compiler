#include "if.h"

#include "../../backend/codegen.h"

If::If(unique_ptr<Expression> condition, unique_ptr<Body> body, vector<unique_ptr<Else>> elses):
  ASTNode(ASTNodeType::IF), m_condition(std::move(condition)), m_body(std::move(body)), m_elses(std::move(elses)) {}

void If::accept(Codegen* generator) const {
  generator->visit(this);
}

void If::print(int indetation_level) const {
  cout << '\n' << setw(indetation_level) << " " << "If Statement{\n";
  m_condition->print(indetation_level + 2);
  m_body->print(indetation_level + 2);
  cout << setw(indetation_level) << " " << "}\n";
  for (const unique_ptr<Else>& current: m_elses){
    current->print(indetation_level);
  }
}

ASTNode* If::getCondition() const {
  return m_condition->getExpression();
}

vector<ASTNode*> If::getBody() const {
  return m_body->getStatements();
}

vector<Else*> If::getElses() const {
  vector<Else*> elses = {};
  for(const unique_ptr<Else>& current: m_elses){
    elses.push_back(current.get());
  }
  return elses;
}


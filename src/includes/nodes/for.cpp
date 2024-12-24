#include "for.h"
#include "ASTNode.h"

For::For(unique_ptr<Variable> initialization, unique_ptr<Expression> condition, unique_ptr<AssignmentOperator> update, unique_ptr<Body> body):
  ASTNode(ASTNodeType::FOR), m_initialization(std::move(initialization)), m_condition(std::move(condition)), m_update(std::move(update)), m_body(std::move(body)) {}
  
void For::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "For Statement{\n";
  m_initialization->print(indentation_level + 2);
  m_condition->print(indentation_level + 2);
  m_update->print(indentation_level + 2);
  m_body->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

Variable* For::getInitialization() const {
  return m_initialization.get();
}

AssignmentOperator* For::getUpdate() const {
  return m_update.get();
}

Expression* For::getCondition() const {
  return m_condition.get();
}

vector<ASTNode*> For::getBody() const {
  return m_body->getStatements();
}
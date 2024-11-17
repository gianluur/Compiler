#include "body.h"

Body::Body(vector<unique_ptr<ASTNode>> statements):
  m_statements(std::move(statements)) {}

void Body::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Body {";
  for(const auto& statement: m_statements){
    statement->print(indentation_level + 2);
  }
  cout << setw(indentation_level) << " " << "}\n";
} 

vector<ASTNode*> Body::getStatements() const {
  if (m_statements.size() == 0)
    return {};

  vector<ASTNode*> statements = {};
  for(const unique_ptr<ASTNode>& statement: m_statements){
    statements.push_back(statement.get());
  }
  return statements;
}
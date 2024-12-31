#include "list_initializer.h"
#include "ASTNode.h"
#include "expression.h"

ListInitializer::ListInitializer(vector<unique_ptr<Expression>> list):
  ASTNode(ASTNodeType::LIST_INITIALIZER), m_list(std::move(list)) {}

void ListInitializer::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "ListInitializer {\n";
  for(const unique_ptr<Expression>& element: m_list)
    element->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

vector<Expression*> ListInitializer::getList() const {
  vector<Expression*> list = {};

  for(const unique_ptr<Expression>& element: m_list)
    list.push_back(element.get());
  return list;
}
#include <vector> 

#include "ASTNode.h"
#include "expression.h"

class ListInitializer: public ASTNode {
public:
  ListInitializer(const vector<unique_ptr<Expression>>& list):
  ASTNode(LIST_INITIALIZER), m_list(std::move(list)) {}

private:
  vector<unique_ptr<Expression>> m_list;
};
#include "variable.h"
#include "ASTNode.h"
#include "expression.h"
#include "list_initializer.h"
#include "struct.h"
#include <string>

Variable::Variable(const Token& keyword, unique_ptr<Type> type, unique_ptr<Identifier> identifier, const bool isMember):
  ASTNode(ASTNodeType::VARIABLE), m_keyword(keyword), m_type(std::move(type)), 
  m_identifier(std::move(identifier)), m_value(make_unique<Expression>()), m_isMember(isMember) {
    analyzeVariable();
  }

Variable::Variable(const Token& keyword, unique_ptr<Type> type, unique_ptr<Identifier> identifier, ValueVariant value,  const bool isMember):
  ASTNode(ASTNodeType::VARIABLE), m_keyword(keyword), m_type(std::move(type)), 
  m_identifier(std::move(identifier)), m_value(std::move(value)), m_isMember(isMember) {
    analyzeVariable();
  }

void Variable::print(int indentation_level) const {
  cout << '\n' << std::setw(indentation_level) << " " << "Variable {\n";
  cout << std::setw(indentation_level + 2) << " " << "keyword: " << m_keyword.lexemes << '\n';
  m_type->print(indentation_level + 2);
  m_identifier->print(indentation_level + 2);
  getValue()->print(indentation_level + 2);
  cout << std::setw(indentation_level) << " " << "}\n";
}

string Variable::getKeyword() const {
  return m_keyword.lexemes;
}

ASTNodeType Variable::getType() const {
  return m_type->getType();
}

ASTNode* Variable::getValue() const {
  if (std::holds_alternative<unique_ptr<Expression>>(m_value))
    return std::get<unique_ptr<Expression>>(m_value).get();
  else 
    return std::get<unique_ptr<ListInitializer>>(m_value).get();
}

bool Variable::isPointer() const {
  return m_type->isPointer();
}

string Variable::getIdentifier() const {
  return m_identifier->toString();
}

string Variable::getTypeToString() const {
  return m_type->toString();
}

void Variable::analyzeVariable() const {
  const ASTNode* value = getValue();
  if (m_type->isStruct()) {

    if (!(value->getNodeType() == ASTNodeType::LIST_INITIALIZER))
      error("In variable declaration: " + getKeyword() + " " + getTypeToString() + " " + getIdentifier() + " is a struct type, so it can only be initialized with a list initializer");
    const vector<Expression*> list = std::get<unique_ptr<ListInitializer>>(m_value)->getList();
    const Struct* structure = std::get<const Struct*>(Scope::getInstance()->find(getTypeToString()).symbol);

    if (list.size() != structure->getMembersSize())
      error("In this variable declaration: " + getKeyword() + " " + getTypeToString() + " " + getIdentifier() + " the numbers elements in the list initializer is different than the struct members required");

    for (int index = 0; index < list.size(); index++) {
      const ASTNodeType memberType = structure->getMember(index)->getType();
      const ASTNodeType elementType = Expression::analyzeExpression(list[index]);

      if (!(memberType == elementType)){
        cout << "mt: " << static_cast<int>(memberType) << " et: " << static_cast<int>(elementType) << '\n';
        error("In variable declaration: " + getKeyword() + " " + getTypeToString() + " " + getIdentifier() + " the " + std::to_string(index + 1) + " element type doesn't match the one in the structure");
      }
    }
  }
  else
    if (Expression::analyzeExpression(value) != NULL && Expression::analyzeExpression(value) != getType())
      error("In variable declaration the value and the type doesn't match");

  if (!m_isMember){
    Scope::getInstance()->declare(m_identifier->toString(), Symbol(this));
  }
}
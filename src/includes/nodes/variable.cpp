#include "variable.h"
#include "ASTNode.h"
#include "expression.h"

Variable::Variable(const Token& keyword, unique_ptr<Type> type, unique_ptr<Identifier> identifier, const bool isMember):
  ASTNode(ASTNodeType::VARIABLE), m_keyword(keyword), m_type(std::move(type)), 
  m_identifier(std::move(identifier)), m_value(make_unique<Expression>()), m_isMember(isMember) {
    analyzeVariable();
  }

Variable::Variable(const Token& keyword, unique_ptr<Type> type, unique_ptr<Identifier> identifier, unique_ptr<Expression> value,  const bool isMember):
  ASTNode(ASTNodeType::VARIABLE), m_keyword(keyword), m_type(std::move(type)), 
  m_identifier(std::move(identifier)), m_value(std::move(value)), m_isMember(isMember) {
    analyzeVariable();
  }

void Variable::print(int indentation_level) const {
  cout << '\n' << std::setw(indentation_level) << " " << "Variable {\n";
  cout << std::setw(indentation_level + 2) << " " << "keyword: " << m_keyword.lexemes << '\n';
  m_type->print(indentation_level + 2);
  m_identifier->print(indentation_level + 2);
  m_value->print(indentation_level + 2);
  cout << std::setw(indentation_level) << " " << "}\n";
}

string Variable::getKeyword() const {
  return m_keyword.lexemes;
}

ASTNodeType Variable::getType() const {
  return m_type->getType();
}

ASTNode* Variable::getValue() const {
  return m_value->getExpression();
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
  if (Expression::analyzeExpression(getValue()) != NULL && Expression::analyzeExpression(getValue()) != getType())
    error("In variable declaration the value and the type doesn't match");

  if (!m_isMember){
    Scope::getInstance()->declare(m_identifier->toString(), Symbol(this));
  }
}
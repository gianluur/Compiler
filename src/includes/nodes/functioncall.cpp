#include "functioncall.h"
#include "expression.h"
#include "function.h"
#include "ASTNode.h"
#include "parameter.h"
#include <cerrno>
#include <memory>
#include <string>

FunctionCall::FunctionCall(unique_ptr<Identifier> identifier, vector<unique_ptr<Expression>> arguments, const bool isInsideExpression):
  ASTNode(ASTNodeType::FUNCTION_CALL), m_identifier(std::move(identifier)), m_arguments(std::move(arguments)), m_isInsideExpression(isInsideExpression) {
    if (!m_isInsideExpression){
      analyzeFunctionCall(this);
    }
  }

void FunctionCall::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Function Call{\n";
  m_identifier->print(indentation_level + 2);
  cout << setw(indentation_level + 2) << " " << "Arguments: \n";
  for(const auto& argument: m_arguments){
    argument->print(indentation_level + 2);
  }
  cout << setw(indentation_level) << " " << "}\n";
}

vector<Expression*> FunctionCall::getArguments() const {
  vector<Expression*> arguments = {};
  for(const unique_ptr<Expression>& argument: m_arguments){
    arguments.push_back(argument.get());
  }
  return arguments;
}

const ASTNodeType FunctionCall::analyzeFunctionCall(const FunctionCall* functionCall) const {
  const string name = functionCall->m_identifier->toString();
  
  if (!Scope::getInstance()->isDeclared(name))
    error("Function call: " + name + " definition wasn't found");
  const Symbol& symbol = Scope::getInstance()->find(name);

  if (symbol.type != ASTNodeType::FUNCTION)
    error("Another symbol has the same identifier as the function call you're calling");
  const Function* function = std::get<const Function*>(symbol.symbol);
  
  const vector<Expression*> arguments = functionCall->getArguments();
  const vector<Parameter*> parameters = function->getParameter();

  if (parameters.size() != arguments.size())
    error("Function call was required to called with: " + std::to_string(parameters.size()) + " but it was called with " + std::to_string(arguments.size()) + " instead");
  
  for(int i = 0; i < parameters.size(); i++)
    if (!Type::AreEquals(parameters[i]->getType(), arguments[i]->getType()))
      error("In function call the " + std::to_string(i + 1) + "# arguments doesn't match the paramter");
  
  return std::get<const Function*>(symbol.symbol)->getType(); //sometimes this value will get discarded
}
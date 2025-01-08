#include "functioncall.h"
#include "function.h"
#include "ASTNode.h"

FunctionCall::FunctionCall(unique_ptr<Identifier> identifier, vector<unique_ptr<Expression>> arguments):
  ASTNode(ASTNodeType::FUNCTION_CALL), m_identifier(std::move(identifier)), m_arguments(std::move(arguments)) {
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

const ASTNodeType FunctionCall::getFunctionCallType(const FunctionCall* functionCall) const {
  const string name = functionCall->m_identifier->toString();
  
  if (!Scope::getInstance()->isDeclared(name))
    error("Function call: " + name + " definition wasn't found");
  const Symbol& symbol = Scope::getInstance()->find(name);

  if (symbol.type != ASTNodeType::FUNCTION)
    error("Another symbol has the same identifier as the function call you're calling");
  return std::get<const Function*>(symbol.symbol)->getType();
}
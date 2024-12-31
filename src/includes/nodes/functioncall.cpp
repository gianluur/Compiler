#include "functioncall.h"
#include "ASTNode.h"

FunctionCall::FunctionCall(unique_ptr<Identifier> identifier, vector<unique_ptr<Expression>> arguments):
  ASTNode(ASTNodeType::FUNCTION_CALL), m_identifier(std::move(identifier)), m_arguments(std::move(arguments)) {}

void FunctionCall::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Function Call{\n";
  m_identifier->print(indentation_level + 2);
  for(const auto& argument: m_arguments){
    argument->print(indentation_level + 2);
  }
  cout << setw(indentation_level) << " " << "}\n";
}

const ASTNodeType FunctionCall::getFunctionCallType(const FunctionCall* functionCall) const {
  const string name = functionCall->m_identifier->toString();
  if (!Scope::getInstance()->isDeclared(name))
    error("Function call: " + name + " definition wasn't found");

  return Scope::getInstance()->find(name).type;
}
#include "functioncall.h"

FunctionCall::FunctionCall(unique_ptr<Identifier> identifier, vector<unique_ptr<Expression>> arguments):
  m_identifier(std::move(identifier)), m_arguments(std::move(arguments)) {}

void FunctionCall::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Function Call{\n";
  m_identifier->print(indentation_level + 2);
  for(const auto& argument: m_arguments){
    argument->print(indentation_level + 2);
  }
  cout << setw(indentation_level) << " " << "}\n";
}
#include "function.h"

Function::Function(unique_ptr<Type> type, unique_ptr<Identifier> identifier, vector<unique_ptr<Parameter>> parameters, unique_ptr<Body> body):
  m_type(std::move(type)), m_identifier(std::move(identifier)), m_parameters(std::move(parameters)), m_body(std::move(body)) {}

void Function::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "Function {\n";
  m_type->print(indentation_level + 2);
  m_identifier->print(indentation_level + 2);
  for(const auto& parameter: m_parameters){
    parameter->print(indentation_level + 2);
  }
  m_body->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

TokenType Function::getType() const {
  return m_type->getType();
}

string Function::getIdentifier() const {
  return m_identifier->toString();
}

vector<Parameter*> Function::getParameter() const {
  vector<Parameter*> parameters;
  for(const unique_ptr<Parameter>& current: m_parameters){
    parameters.push_back(current.get());
  }
  return parameters;
}
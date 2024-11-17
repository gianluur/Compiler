#include "parameter.h"

Parameter::Parameter(unique_ptr<Type> type, unique_ptr<Identifier> name)
  : m_type(std::move(type)), m_identifier(std::move(name)) {}

void Parameter::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "Parameter {\n";
  m_type->print(indentation_level + 2);
  m_identifier->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

TokenType Parameter::getType() const {
  return m_type->getType();
}

string Parameter::getIdentifier() const {
  return m_identifier->toString();
}
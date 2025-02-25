#include "parameter.h"
#include "ASTNode.h"

#include "../../backend/codegen.h"

Parameter::Parameter(unique_ptr<Type> type, unique_ptr<Identifier> name): 
  ASTNode(ASTNodeType::PARAMETER), m_type(std::move(type)), m_identifier(std::move(name)) {}

void Parameter::accept(Codegen* generator) const {
  generator->visit(this);
}

void Parameter::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "Parameter {\n";
  m_type->print(indentation_level + 2);
  m_identifier->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

ASTNodeType Parameter::getType() const {
  return m_type->getType();
}

string Parameter::getIdentifier() const {
  return m_identifier->toString();
}
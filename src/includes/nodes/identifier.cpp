#include "identifier.h"

Identifier::Identifier(const Token& token): 
  m_str(token.lexemes) {}

void Identifier::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Identifier: " << m_str << '\n';
}

string Identifier::toString() const {
  return m_str;
}
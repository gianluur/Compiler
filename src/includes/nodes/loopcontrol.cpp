#include "loopcontrol.h"

LoopControl::LoopControl(const Token& token, const TokenType scope): 
  m_str(std::move(token.lexemes)), m_scope(scope) {}

void LoopControl::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "LoopControl {\n";
  cout << setw(indentation_level + 2) << " " << m_str << '\n';
  cout << setw(indentation_level) << " " << "}\n";
}

string LoopControl::getKeyword() const {
  return m_str;
}

TokenType LoopControl::getScope() const {
  return m_scope;
}
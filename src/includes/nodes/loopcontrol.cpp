#include "loopcontrol.h"

LoopControl::LoopControl(const Token& token): m_str(std::move(token.lexemes)) {}

void LoopControl::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "LoopControl {\n";
  cout << setw(indentation_level + 2) << " " << m_str << '\n';
  cout << setw(indentation_level) << " " << "}\n";
}

string LoopControl::getKeyword() const {
  return m_str;
}
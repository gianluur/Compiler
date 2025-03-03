#include "ASTNode.h"
#include "loopcontrol.h"

#include "../../backend/codegen.h"
#include <token.hpp>

LoopControl::LoopControl(const Token& token, const enum TokenType scope): 
  ASTNode(ASTNodeType::LOOP_CONTROL), m_str(std::move(token.lexemes)), m_scope(scope) {
    analyzeLoopControl();
  }

void LoopControl::accept(Codegen* generator) const {
  generator->visit(this);
}

void LoopControl::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "LoopControl {\n";
  cout << setw(indentation_level + 2) << " " << m_str << '\n';
  cout << setw(indentation_level) << " " << "}\n";
}

string LoopControl::getKeyword() const {
  return m_str;
}

enum TokenType LoopControl::getScope() const {
  return m_scope;
}

void LoopControl::analyzeLoopControl() const {
  if (m_scope == TokenType::NOTHING)
    error(m_str + " statement can't be outside a function scope");
}
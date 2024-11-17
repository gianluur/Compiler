#include "literal.h"

Literal::Literal(const Token& token) : m_type(token.type), m_str(token.lexemes) {}

Literal::Literal() : m_type(TokenType::NULL), m_str("null") {}

void Literal::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Value: " << m_str << '\n';
}

TokenType Literal::getType() const {
  return m_type;
}

bool Literal::isInteger() const {
  return m_type == TokenType::LITERAL_INTEGER;
}

bool Literal::isFloat() const {
  return m_type == TokenType::LITERAL_FLOAT;
}

bool Literal::isBool() const {
  return m_type == TokenType::BOOL;
}

bool Literal::isChar() const {
  return m_type == TokenType::CHAR;
}

bool Literal::isNull() const {
  return m_type == TokenType::NULL;
}

string Literal::toString() const {
  return m_str;
}

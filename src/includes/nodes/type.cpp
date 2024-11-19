#include "type.h"

Type::Type(const Token& token, const bool isPointer): 
  m_type(token.type), m_str(token.lexemes), m_isPointer(isPointer) {}

void Type::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Type: " << m_str << '\n';
}

TokenType Type::getType() const {
  return m_type;
}

bool Type::isInteger() const {
  return m_type >= TokenType::INTEGERS_BEGIN && m_type <= TokenType::INTEGERS_END;
}

bool Type::isFloat() const {
  return m_type >= TokenType::FLOAT_BEGIN && m_type <= TokenType::FLOAT_END;
}

bool Type::isBool() const {
  return m_type == TokenType::BOOL;
}

bool Type::isChar() const {
  return m_type == TokenType::CHAR;
}

bool Type::isNull() const {
  return m_type == TokenType::NULL;
}

bool Type::isPointer() const {
  return m_isPointer;
}

string Type::toString() const {
  return m_str;
}

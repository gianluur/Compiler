#include "type.h"
#include "ASTNode.h"

Type::Type(const Token& token, const bool isPointer): 
  ASTNode(Type::TokenTypeToASTNodeType(token.type)), m_type(token.type), m_str(token.lexemes), m_isPointer(isPointer) {}

void Type::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Type: " << m_str << '\n';
}

ASTNodeType Type::getType() const {
  return getNodeType();
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

ASTNodeType Type::TokenTypeToASTNodeType(const TokenType type) const {
  switch (type) {
    case TokenType::INT:
      return ASTNodeType::INT;
    case TokenType::INT8:
      return ASTNodeType::INT8;
    case TokenType::INT16:
      return ASTNodeType::INT16;
    case TokenType::INT32:
      return ASTNodeType::INT32;
    case TokenType::INT64:
      return ASTNodeType::INT64;
    case TokenType::UINT:
      return ASTNodeType::UINT;
    case TokenType::UINT8:
      return ASTNodeType::UINT8;
    case TokenType::UINT16:
      return ASTNodeType::UINT16;
    case TokenType::UINT32:
      return ASTNodeType::UINT32;
    case TokenType::UINT64:
      return ASTNodeType::UINT64;
    case TokenType::FLOAT:
      return ASTNodeType::FLOAT;
    case TokenType::FLOAT32:
      return ASTNodeType::FLOAT32;
    case TokenType::FLOAT64:
      return ASTNodeType::FLOAT64;
    case TokenType::CHAR:
      return ASTNodeType::CHAR;
    case TokenType::STRING:
      return ASTNodeType::STRING;
    case TokenType::BOOL:
      return ASTNodeType::BOOL;
    case TokenType::NULL:
      return ASTNodeType::NULL;
    default:
      error("Unexpected error when converting type from TokenType to ASTNodeType");
  }
}

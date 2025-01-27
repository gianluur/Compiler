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

bool Type::isStruct() const {
  return m_type == TokenType::IDENTIFIER;
}

bool Type::AreEquals(const ASTNodeType type, const ASTNodeType valueType) {
  if (type == valueType)
    return true;

  //Checks if it's smaller types, to avoid that stuff like int and int32 gives a mismatch type
  static const std::unordered_set<ASTNodeType> intTypes = {
    INT, INT8, INT16, INT32, INT64,
    UINT, UINT8, UINT16, UINT32, UINT64
  };

  static const std::unordered_set<ASTNodeType> floatTypes = {
    FLOAT, FLOAT32, FLOAT64
  };

  return ((intTypes.find(type) != intTypes.end()) && (intTypes.find(valueType) != intTypes.end())) ||
         ((floatTypes.find(type) != floatTypes.end()) && (floatTypes.find(valueType) != floatTypes.end()));
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
    case TokenType::IDENTIFIER:
      return ASTNodeType::IDENTIFIER; // this is actually a struct like type
    default:
      error("Unexpected error when converting type from TokenType to ASTNodeType");
  }
}

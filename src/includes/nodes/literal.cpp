#include "literal.h"
#include "ASTNode.h"

#include "../../backend/codegen.h"

Literal::Literal(const Token& token): 
  ASTNode(Literal::LiteralTokenToASTNodeTypeLiteral(token.type)), m_type(token.type), m_str(token.lexemes) {}

Literal::Literal(): 
  ASTNode(ASTNodeType::NOTHING), m_type(TokenType::NOTHING), m_str("null") {}

void Literal::accept(Codegen* generator) const {
  generator->visit(this);
}

void Literal::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Value: " << m_str << '\n';
}

enum TokenType Literal::getType() const {
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
  return m_type == TokenType::NOTHING;
}

string Literal::toString() const {
  return m_str;
}

ASTNodeType Literal::LiteralTokenToASTNodeTypeLiteral(const enum TokenType tokenType) {
  switch (tokenType) {
    case TokenType::LITERAL_INTEGER:
      return ASTNodeType::LITERAL_INTEGER;
      break;
    case TokenType::LITERAL_FLOAT:
      return ASTNodeType::LITERAL_FLOAT;
      break;
    case TokenType::LITERAL_CHARACTER:
      return ASTNodeType::LITERAL_CHARACTER;
      break;
    case TokenType::LITERAL_STRING:
      return ASTNodeType::LITERAL_STRING;
      break;
    case TokenType::LITERAL_BOOLEAN:
      return ASTNodeType::LITERAL_BOOLEAN;
      break;
    default:
      error("Unexpected error in TokenTypeToASTNodeType()");
      break;      
  }
}

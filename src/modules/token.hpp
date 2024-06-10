#include <string>
using std::string;

enum class TokenType {

  INVALID,
  SEMICOLON,
  NUMBER,

  //ASSIGNMENT
  ASSIGNMENT,
  ADDITION_ASSIGNMENT,
  SUBTRACTION_ASSIGNMENT,
  MULTIPLICATION_ASSIGNMENT,
  DIVISION_ASSIGNMENT,
  MODULUS_ASSIGNMENT,

  //MATH

  ADDITION,
  SUBTRACTION,
  MULTIPLICATION,
  DIVISION,
  MODULUS,

  //PARENTHESIS

  LPAREN,
  RPAREN,
  LBRACKET,
  RBRACKET,
  LCURLY,
  RCURLY,

  //COMPARISON

  EQUALS,
  GREATER,
  LESS,
  GREATER_EQUAL,
  LESS_EQUAL,


};

struct Token {
  TokenType type;
  string lexemes;
  double value;

  Token(TokenType type, string lexemes) : type(type), lexemes(lexemes) {}
  Token(TokenType type, string lexemes, double value): type(type), lexemes(lexemes), value(value) {}
  Token(TokenType type, string lexemes, double value, size_t pos): type(type), lexemes(lexemes), value(value) {}
};
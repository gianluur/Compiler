#pragma once

//REMOVING SOME ANNOYING MACROS, THAT FILL MY NAMESPACE

#undef NULL 
#undef EOF

#include <string>
using std::string;

enum class TokenType {

  //MISCELLANEOUS
  INVALID,
  EOF,
  SEMICOLON,
  COLON,
  COMMA,
  DOT,
  IDENTIFIER,

  //TYPES
  INT,
  FLOAT,
  CHAR,
  STRING,
  BOOL,
  NULL,

  //LITERALS
  LITERAL_INTEGER,
  LITERAL_FLOAT,
  LITERAL_CHARACTER,
  LITERAL_STRING,
  LITERAL_BOOLEAN,

  //KEYWORDS
  VAR,
  CONST,
  IF,
  ELSE,
  DO,
  WHILE,
  FOR,
  BREAK,
  CONTINUE,
  FUNC,
  RETURN,
  STRUCT,

  //LOGICAL
  AND,
  OR,
  NOT,

  //ASSIGNMENT
  ASSIGNMENT,
  ADDITION_ASSIGNMENT,
  SUBTRACTION_ASSIGNMENT,
  MULTIPLICATION_ASSIGNMENT,
  DIVISION_ASSIGNMENT,
  MODULUS_ASSIGNMENT,

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

  //MATH
  ADDITION,
  SUBTRACTION,
  MULTIPLICATION,
  DIVISION,
  MODULUS,

};

struct Token {
  TokenType type;
  string lexemes;
  size_t start;
  size_t end;


  Token(TokenType type, string lexemes) : type(type), lexemes(lexemes) {}
  Token(TokenType type, string lexemes, double value): type(type), lexemes(lexemes){}

  //ADD START AND END OF TEXT AND LITERALS TOKEN
  Token(TokenType type, string lexemes, size_t start, size_t end): 
    type(type), lexemes(lexemes), start(start), end(end)  {}
  Token(TokenType type, string lexemes, double value, size_t start, size_t end): 
    type(type), lexemes(lexemes), start(start), end(end)  {}
};
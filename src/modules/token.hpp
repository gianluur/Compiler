#pragma once

//REMOVING SOME ANNOYING MACROS, THAT FILL MY NAMESPACE
#pragma push_macro("EOF")
#undef EOF 

#pragma push_macro("NULL")
#undef NULL 

#include <string>
using std::string;

enum class TokenType {

  INVALID,
  SEMICOLON,
  IDENTIFIER,
  TRUE,
  FALSE,

  //LITERALS
  LITERAL_INTEGER,
  LITERAL_FLOAT,
  LITERAL_CHARACTER,
  LITERAL_STRING,
  LITERAL_BOOLEAN,
  LITERAL_NULL,

  //KEYWORDS
  LET,
  CONST,
  IF,
  ELSE,
  DO,
  WHILE,
  FOR,
  RETURN,
  BREAK,
  CONTINUE,
  FUNC,
  STRUCT,

  //TYPES
  INT,
  FLOAT,
  CHAR,
  STRING,
  BOOL,

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
#pragma once

#include <string>

//REMOVING SOME ANNOYING MACROS, THAT FILL MY NAMESPACE

#undef NULL 
#undef EOF

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
  ARRAY_INT,
  ARRAY_FLOAT,
  ARRAY_CHAR,
  ARRAY_STRING,
  ARRAY_BOOL,
  
  INT,
  INT8,
  INT16,
  INT32,
  INT64,

  UINT,
  UINT8,
  UINT16,
  UINT32,
  UINT64,

  FLOAT,
  FLOAT32,
  FLOAT64,

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
  NOT_EQUAL,
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
  size_t line;

  Token(): type(TokenType::INVALID), lexemes("") {}
  Token(TokenType type, string lexemes): type(type), lexemes(lexemes) {}
  Token(TokenType type, string lexemes, size_t line): 
    type(type), lexemes(lexemes), line(line) {}
};
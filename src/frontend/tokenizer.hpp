#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include "../includes/token.hpp"
#include "../includes/error.hpp"

using std::cout, std::cerr;
using std::string, std::string_view, std::vector, std::unordered_map, std::unordered_set;
using std::pair;

class Tokenizer{
public:
  Tokenizer(const string source_code):
    m_src(source_code), index(0), line(1) {
      cout << "----- Tokenizer -----\n\n";
      tokenize(); 
      cout << "\n---------------------\n\n";
    }

  ~Tokenizer(){}

  vector<Token> getTokens() const {
    return m_tokens;
  }

private:
  string m_src;
  vector<Token> m_tokens;

  size_t index;
  size_t line;

  const unordered_map<char, TokenType> miscellaneousMap = {
    { '(', TokenType::LPAREN },
    { ')', TokenType::RPAREN },
    { '[', TokenType::LBRACKET },
    { ']', TokenType::RBRACKET },
    { '{', TokenType::LCURLY },
    { '}', TokenType::RCURLY },
    { ';', TokenType::SEMICOLON },
    { ',', TokenType::COMMA },
    { '.', TokenType::DOT }
  };

  const unordered_map<string, TokenType> keywordMap = {
    { "var", TokenType::VAR },
    { "const", TokenType::CONST },
    { "int8", TokenType::INT8 },
    { "int16", TokenType::INT16 },
    { "int32", TokenType::INT32 },
    { "int64", TokenType::INT64 },
    { "int", TokenType::INT },
    { "uint8", TokenType::UINT8 },
    { "uint16", TokenType::UINT16 },
    { "uint32", TokenType::UINT32 },
    { "uint64", TokenType::UINT64 },
    { "uint", TokenType::UINT},
    { "float32", TokenType::FLOAT32 },
    { "float64", TokenType::FLOAT64 },
    { "float", TokenType::FLOAT },
    { "char", TokenType::CHAR },
    { "string", TokenType::STRING },
    { "bool", TokenType::BOOL },
    { "null", TokenType::NULL },
    { "if", TokenType::IF },
    { "else", TokenType::ELSE },
    { "do", TokenType::DO },
    { "while", TokenType::WHILE },
    { "for", TokenType::FOR },
    { "break", TokenType::BREAK },
    { "continue", TokenType::CONTINUE },
    { "func", TokenType::FUNC },
    { "return", TokenType::RETURN },
    { "struct", TokenType::STRUCT },
    { "true", TokenType::LITERAL_BOOLEAN },
    { "false", TokenType::LITERAL_BOOLEAN }
  };

  const unordered_map<char, TokenType> singleCharOperatorMap = {
    { '+', TokenType::ADDITION },
    { '-', TokenType::SUBTRACTION },
    { '*', TokenType::MULTIPLICATION },
    { '/', TokenType::DIVISION },
    { '%', TokenType::MODULUS },
    { '!' , TokenType::NOT},
    { '<' , TokenType::LESS},
    { '>' , TokenType::GREATER},
    { '=', TokenType::ASSIGNMENT}
  };

  const unordered_map<string, TokenType> doubleCharOperatorMap = {
    { "&&", TokenType::AND },
    { "||", TokenType::OR },
    { "==", TokenType::EQUALS },
    { "!=", TokenType::NOT_EQUAL },
    { ">=", TokenType::GREATER_EQUAL },
    { "<=", TokenType::LESS_EQUAL },
    { "+=", TokenType::ADDITION_ASSIGNMENT },
    { "-=", TokenType::SUBTRACTION_ASSIGNMENT },
    { "*=", TokenType::MULTIPLICATION_ASSIGNMENT },
    { "/=", TokenType::DIVISION_ASSIGNMENT },
    { "%=", TokenType::MODULUS_ASSIGNMENT }
  };

  void print() const {
    for(const Token& token : m_tokens){
      cout << "< Type: " << int(token.type) << " Lexemes: " << token.lexemes << " Line: " << token.line << " >\n" ;
    }
  }

  void tokenize(){
    for(index = 0; index < m_src.size(); index++){
      const char current = m_src[index];
      if (std::isspace(current))
        continue;
      
      if (current == '\n'){
        line++;
        continue;
      }
      
      if (isNumber(current))
        m_tokens.push_back(tokenNumber(current));

      else if (isText(current))
        m_tokens.push_back(tokenText(current));
      
      else if (isOperator(current))
        m_tokens.push_back(tokenOperator(current));
        
      else if (isMiscellaneous(current))
        m_tokens.push_back(tokenMiscellaneous(current));
      
      else if (isSingleQuote(current))
        m_tokens.push_back(tokenChar(current));
      
      else if (isDoubleQuote(current))
        m_tokens.push_back(tokenString(current));
    }
    print();
  }

  char nextChar(){
    if (++index >= m_src.size()) 
      return '\0';
    return m_src.at(index);
  }

  char peekNextChar() const {
    if (index + 1 >= m_src.size())
      error("out of bounds!", line);
    return m_src.at(index + 1);
  }

  void invalidToken(const char& character, const string& message = "") const {
    if (message == "")
      error("Invalid token detected: '" + string(1, character) + "'", line);
    else 
      error(message, line);
  }
  
  bool isText(const char& current) const {
    return isalnum(current) || current == '_';
  }

  bool isNumber(const char& current) const {
    return std::isdigit(current) || (current == '-' && std::isdigit(peekNextChar()));
  }

  bool isMiscellaneous(const char& current) const {
    return miscellaneousMap.find(current) != miscellaneousMap.end();
  }

  bool isSingleQuote(const char& current) const {
    return current == '\'';
  }

  bool isDoubleQuote(const char& current) const {
    return current == '\"';
  }

  bool isOperator(const char& current) {
    if (singleCharOperatorMap.find(current) != singleCharOperatorMap.end()){
      if (peekNextChar() == '=')
        return doubleCharOperatorMap.find(string(1, current) + '=') != doubleCharOperatorMap.end();
      if ((current == '&' || current == '|') && peekNextChar() == current)
        return doubleCharOperatorMap.find(string(2, current)) != doubleCharOperatorMap.end();
      return true;
    }
    return false;
  } 

  string getText(const char& character){
    string text = string(1, character);

    while (isText(peekNextChar())) {
      text += nextChar();
    }

    return text;
  }

  Token tokenMiscellaneous(const char& character) const {
    return Token(miscellaneousMap.at(character), string(1, character), line);
  }

  Token tokenNumber(const char& character){
    string token = character == '-' ? "-" : string(1, character);
    
    char current; uint8_t dots = 0;
    while(std::isdigit(peekNextChar()) || peekNextChar() == '.'){
      current = nextChar();

      if (current == '.' && (dots += 1) > 1)
        invalidToken(character, "Floats can only have one decimal point");

      token += current;
    }

    if (dots == 0)
      return Token(TokenType::LITERAL_INTEGER, token, line);
    else
      return Token(TokenType::LITERAL_FLOAT, token , line);
  }

  Token tokenChar(const char& character){
    string token = string(1, character) + nextChar();

    if (!(peekNextChar() == '\''))
      invalidToken(character, "Missing char closing quote");
    token += nextChar();

    return Token(TokenType::LITERAL_CHARACTER, token, line);
  }

  Token tokenString(const char& character){
    string token = string(1, character);
    char current;
    do {
      current = nextChar();
      token += current;
    } while(current != '\"' && index < m_src.size());

    if (current != '\"')
      invalidToken(character, "Missing string closing quote");

    return Token(TokenType::LITERAL_STRING, token, line);
  }

  Token tokenOperator(const char& character){
    string text = string(1, character);
    if (singleCharOperatorMap.find(character) != singleCharOperatorMap.end() && peekNextChar() == '=') 
      text += nextChar();
    else if ((character == '&' || character == '|') && peekNextChar() == character)
      text += nextChar();

    if (doubleCharOperatorMap.find(text) != doubleCharOperatorMap.end())
      return Token(doubleCharOperatorMap.at(text), text, line);
    else
      return Token(singleCharOperatorMap.at(character), text, line);
  }

  Token tokenText(const char& character){
    const string text = getText(character);
    if (keywordMap.find(text) != keywordMap.end())
      return Token(keywordMap.at(text), text, line);
    else 
      return Token(TokenType::IDENTIFIER, text, line);
  }
};
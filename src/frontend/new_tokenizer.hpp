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

#undef NULL

using std::cout, std::cerr;
using std::string, std::string_view, std::vector, std::unordered_map, std::unordered_set;
using std::pair;

class Tokenizer{
public:
  Tokenizer(const string_view source_code):
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
  string_view m_src;
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

    const unordered_map<string_view, TokenType> keywordMap = {
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

  const unordered_map<string_view, TokenType> operatorMap = {
    { "+", TokenType::ADDITION },
    { "-", TokenType::SUBTRACTION },
    { "*", TokenType::MULTIPLICATION },
    { "/", TokenType::DIVISION },
    { "%", TokenType::MODULUS },
    { "==", TokenType::EQUALS },
    { "!=", TokenType::NOT_EQUAL },
    { ">=", TokenType::GREATER_EQUAL },
    { "<=", TokenType::LESS_EQUAL },
    { "&&", TokenType::AND },
    { "||", TokenType::OR },
    { "!" , TokenType::NOT},
    { "+=", TokenType::ADDITION_ASSIGNMENT },
    { "-=", TokenType::SUBTRACTION_ASSIGNMENT },
    { "*=", TokenType::MULTIPLICATION_ASSIGNMENT },
    { "/=", TokenType::DIVISION_ASSIGNMENT },
    { "%=", TokenType::MODULUS_ASSIGNMENT }
  };

  void tokenize(){
    for(const char& current: m_src){
      if (current == ' ')
        continue;
      
      if (current == '\n'){
        line++;
        continue;
      }

      Token token;

      if ((token = isMiscellaneous(current)).type != TokenType::INVALID){
        
      }
      else if (isText(current)) {
        
      }

      else if (std::isalnum(current) || current == '_'){
        tokenKeyword(current);
      }

      else
        invalidToken(current);

      m_tokens.push_back(token);
    }
  }

  char nextChar() const{
    if (index + 1 >= m_src.size()) 
      return '\0';
    return m_src.at(index + 1);
  }  

  void advance() {
    index += 1;
  }

  void invalidToken(const char& character, const string& message = ""){
    if (message == "")
      error("Invalid token detected: '" + string(1, character) + "'", line);
    else 
      error(message, line);
  }
  
  bool isText(const char& current) const {
    return std::isdigit(current) || current == '-' && std::isdigit(current);
  }

  string_view getText(const char& character){
    string text = string(1, character);
    while(isText(nextChar())){
      text += string(1, nextChar());
      advance();
    }
    return string_view(text);
  }

  Token isMiscellaneous(const char& character) const {
    switch(character) {
      case '(': 
        return Token(TokenType::LPAREN, string(1, character));
      case ')': 
        return Token(TokenType::RPAREN, string(1, character));
      case '[': 
        return Token(TokenType::LBRACKET, string(1, character));
      case ']': 
        return Token(TokenType::RBRACKET, string(1, character));
      case '{': 
        return Token(TokenType::LCURLY, string(1, character));
      case '}': 
        return Token(TokenType::RCURLY, string(1, character));
      case ';': 
        return Token(TokenType::SEMICOLON, string(1, character));
      case ',': 
        return Token(TokenType::COMMA, string(1, character));
      case '.': 
        return Token(TokenType::DOT, string(1, character));
    }
  }

  Token isOperator(const string_view character){

  }

  Token tokenKeyword(const char character){
    const string_view text = getText(character);
    if (keywordMap.find(text) != keywordMap.end())
      invalidToken(character);
    return Token(keywordMap.at(text), string(text), line); //remove the need for string

  }


};
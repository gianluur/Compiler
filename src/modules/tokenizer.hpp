#include <iostream>
#include <cstdint>
#include <vector>

#include "token.hpp"

using std::cout, std::cerr, std::endl;
using std::string, std::vector;

class Tokenizer {
private:
  vector<Token> m_tokens;

  vector<Token> tokenize(string& src){
    vector<Token> m_tokens = {};
    for(size_t i = 0; i < src.size(); i++){
      char currentChar = src.at(i);
      
      if (isdigit(currentChar)) numberToken(src, currentChar, i); 
      
      else {

        parenthesisToken(currentChar, i);
        mathOperatorToken(src, currentChar, i); 
        assignmentOperatorToken(src, currentChar, i);
        comparisonOperatorToken(src, currentChar, i);
      }
        //should be invalid here
    }
    return m_tokens;
  }

  void numberToken(string& src, char currentChar, size_t& i){
    string number = "";
    int8_t dotCount = 0;
    while (i < src.size() && (isdigit(src.at(i)) || (src.at(i) == '.'))){

      if (src.at(i) == '.' && dotCount == 0) dotCount++;
      else if (src.at(i) == '.' && dotCount > 1) invalidToken(currentChar, i);
      
      number += src.at(i);
      i++;
    }
    i--;
    m_tokens.emplace_back(Token(TokenType::NUMBER, number));
  }

  void parenthesisToken(char currentChar, size_t i){
    switch (currentChar){
      case '(':
        m_tokens.emplace_back(Token(TokenType::LPAREN, string(1, currentChar)));
        break;

      case ')':
        m_tokens.emplace_back(Token(TokenType::RPAREN, string(1, currentChar)));
        break;

      case '[':
        m_tokens.emplace_back(Token(TokenType::LBRACKET, string(1, currentChar)));
        break;

      case ']':
        m_tokens.emplace_back(Token(TokenType::RBRACKET, string(1, currentChar)));
        break;

      case '{':
        m_tokens.emplace_back(Token(TokenType::LCURLY, string(1, currentChar)));
        break;

      case '}':
        m_tokens.emplace_back(Token(TokenType::RCURLY, string(1, currentChar)));
        break;
      
      default:
        break;
    }
  }

  void mathOperatorToken(string& src, char currentChar, size_t i){
    //this code is so weird lmao the condition only works if you don't use .at() and if i + 1 <= src.size() 

    if (string("+-*/%").find(currentChar) != string::npos && i + 1 <= src.size() && src[i + 1] != '='){
      switch (currentChar){
        case '+':
          m_tokens.emplace_back(Token(TokenType::ADDITION, string(1, currentChar)));
          break;

        case '-':
          m_tokens.emplace_back(Token(TokenType::SUBTRACTION, string(1, currentChar)));
          break;
        
        case '*':
          m_tokens.emplace_back(Token(TokenType::MULTIPLICATION, string(1, currentChar)));
          break;

        case '/':
          m_tokens.emplace_back(Token(TokenType::DIVISION, string(1, currentChar)));
          break;

        case '%':
          m_tokens.emplace_back(Token(TokenType::MODULUS, string(1, currentChar)));
          break;

        default:
          break;
      }
    }
  }

  void assignmentOperatorToken(string& src, char currentChar, size_t& i){
    if (currentChar == '=' && i + 1 <= src.size() && src[i + 1] != '='){
      m_tokens.emplace_back(Token(TokenType::ASSIGNMENT, string(1, currentChar)));
    }

    else if (string("+-*/%").find(currentChar) != string::npos && i + 1 <= src.size() && src[i + 1] == '='){
      string op = string(1, currentChar) + "=";

      switch (currentChar){
        case '+':
          m_tokens.emplace_back(Token(TokenType::ADDITION_ASSIGNMENT, op));
          break;

        case '-':
          m_tokens.emplace_back(Token(TokenType::SUBTRACTION_ASSIGNMENT, op));
          break;
        
        case '*':
          m_tokens.emplace_back(Token(TokenType::MULTIPLICATION_ASSIGNMENT, op));
          break;

        case '/':
          m_tokens.emplace_back(Token(TokenType::DIVISION_ASSIGNMENT, op));
          break;

        case '%':
          m_tokens.emplace_back(Token(TokenType::MODULUS_ASSIGNMENT, op));
          break;

        default:
          break;
      }
      i++;
    }
  }

  void comparisonOperatorToken(string& src, char currentChar, size_t& i){
    string op = string(1, currentChar);

    if (currentChar == '>'){
      if (i + 1 < src.size() && src.at(i + 1) == '=') {
        op += "=";
        m_tokens.emplace_back(Token(TokenType::GREATER_EQUAL, op));
        i++;
      }
      else m_tokens.emplace_back(Token(TokenType::GREATER, op));
    }
    
    else if (currentChar == '<'){
      if (i + 1 < src.size() && src.at(i + 1) == '=') {
        op += "=";
        m_tokens.emplace_back(Token(TokenType::LESS_EQUAL, op));
        i++;
      }
      else m_tokens.emplace_back(Token(TokenType::LESS, op));
    }

    else if (currentChar == '=' && i + 1 < src.size() && src.at(i + 1) == '='){
      op += "=";
      m_tokens.emplace_back(Token(TokenType::EQUALS, op));
      i++;
    }
  }

  void invalidToken(char currentChar, size_t i){
    m_tokens.emplace_back(Token(TokenType::INVALID, string(1, currentChar)));
    cerr << "Invalid token detected at index: " << i << endl;
    exit(EXIT_FAILURE);
  }

  void print(){
    for(Token& token : m_tokens){
      cout << "Type: " << int(token.type) << " Lexemes: " << token.lexemes << endl;
    }
  }

public:
  Tokenizer(string& src): m_tokens(tokenize(src)) { print(); }

  vector<Token> getTokens() const { return m_tokens; }
};


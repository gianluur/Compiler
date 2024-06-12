#include <iostream>
#include <cstdint>
#include <vector>

#include "token.hpp"

using std::cout, std::cerr, std::endl;
using std::string, std::vector;

class Tokenizer{
public:
  Tokenizer(string& src): m_src(src) { tokenize(); }

  vector<Token> getTokens() const { return m_tokens; }

  void tokenize(){
    for(i = 0; i < m_src.size(); i++){
      char currentChar = m_src.at(i);

      if (isDigit(currentChar)) tokenNumber(currentChar);
      
      else if (isParenthesis(currentChar)) tokenParenthesis(currentChar);

      else if (isMathOperator(currentChar)) tokenMathOperator(currentChar);

      else if (isAssignmentOperator(currentChar)) tokenAssignmentOperator(currentChar);

      else if (isComparisonOperator(currentChar)) tokenComparisonOperator(currentChar);

      //else invalidToken(currentChar);
    }
  }

  void print(){
    for(Token& token : m_tokens){
      cout << "Type: " << int(token.type) << " Lexemes: " << token.lexemes << endl;
    }
  }

private:
  vector<Token> m_tokens;
  string m_src;
  size_t i = 0;

  bool contains(const string& charset, const char& currentChar){
    return charset.find(currentChar) != string::npos;
  }

  char nextChar(){
    return i + 1 < m_src.size() && m_src.at(i + 1);
  }

  bool isNextChar(const char& charToCheck){
    return i + 1 < m_src.size() && m_src.at(i + 1) == charToCheck;
  }

  bool isDigit(const char& currentChar){
    return contains("0123456789", currentChar);
  }

  bool isParenthesis(const char& currentChar){
    return contains("()[]{}", currentChar);
  }

  bool isMathOperator(const char& currentChar){
    return contains("+-*/%", currentChar) && !isNextChar('=');
  }

  bool isAssignmentOperator(const char& currentChar){
    return (currentChar == '=' && !isNextChar('=')) || (contains("+-*/%", currentChar) && isNextChar('='));
  }

  bool isComparisonOperator(const char& currentChar){
    return contains("<>", currentChar) || (currentChar == '=' && isNextChar('=')); 
  }

  void invalidToken(const char& currentChar){
    m_tokens.emplace_back(Token(TokenType::INVALID, string(1, currentChar)));
    cerr << "Invalid token detected at index: " << i << endl;
    exit(EXIT_FAILURE);
  }

  void tokenNumber(const char& currentChar){
    string number = "";
    int8_t dotCount = 0;
    while (i < m_src.size() && (isdigit(m_src.at(i)) || (m_src.at(i) == '.'))){

      if (m_src.at(i) == '.' && dotCount == 0) dotCount++;
      else if (m_src.at(i) == '.' && dotCount > 1) invalidToken(currentChar);
      
      number += m_src.at(i);
      i++;
    }
    i--;
    m_tokens.emplace_back(Token(TokenType::NUMBER, number));
  }

  void tokenParenthesis(const char& currentChar){
    const string token = string(1, currentChar);

    switch (currentChar){
      case '(':
        m_tokens.emplace_back(Token(TokenType::LPAREN, token));
        break;

      case ')':
        m_tokens.emplace_back(Token(TokenType::RPAREN, token));
        break;

      case '[':
        m_tokens.emplace_back(Token(TokenType::LBRACKET, token));
        break;

      case ']':
        m_tokens.emplace_back(Token(TokenType::RBRACKET, token));
        break;

      case '{':
        m_tokens.emplace_back(Token(TokenType::LCURLY, token));
        break;

      case '}':
        m_tokens.emplace_back(Token(TokenType::RCURLY, token));
        break;
      
      default:
        break;
    }
  }

  void tokenMathOperator(const char& currentChar){
    const string token = string(1, currentChar);

    switch (currentChar){
      case '+':
        m_tokens.emplace_back(Token(TokenType::ADDITION, token));
        break;

      case '-':
        m_tokens.emplace_back(Token(TokenType::SUBTRACTION, token));
        break;
      
      case '*':
        m_tokens.emplace_back(Token(TokenType::MULTIPLICATION, token));
        break;

      case '/':
        m_tokens.emplace_back(Token(TokenType::DIVISION, token));
        break;

      case '%':
        m_tokens.emplace_back(Token(TokenType::MODULUS, token));
        break;

      default:
        break;
    }
  }

  void tokenAssignmentOperator(const char& currentChar){
    
    if (currentChar == '=') {
      const string token = string(1, currentChar);
      m_tokens.emplace_back(Token(TokenType::ASSIGNMENT, token)); 
    }

    else {
      const string token = string(1, currentChar) + "=";

      switch (currentChar){
        case '+':
          m_tokens.emplace_back(Token(TokenType::ADDITION_ASSIGNMENT, token));
          break;

        case '-':
          m_tokens.emplace_back(Token(TokenType::SUBTRACTION_ASSIGNMENT, token));
          break;
        
        case '*':
          m_tokens.emplace_back(Token(TokenType::MULTIPLICATION_ASSIGNMENT, token));
          break;

        case '/':
          m_tokens.emplace_back(Token(TokenType::DIVISION_ASSIGNMENT, token));
          break;

        case '%':
          m_tokens.emplace_back(Token(TokenType::MODULUS_ASSIGNMENT, token));
          break;

        default:
          break;
      }
      i++;
    }
  }

  void tokenComparisonOperator(const char& currentChar){

    if (currentChar == '>'){
      if (isNextChar('=')){
        const string token = string(1, currentChar) + "=";
        m_tokens.emplace_back(Token(TokenType::GREATER_EQUAL, token));
        i++;
      }
      else m_tokens.emplace_back(Token(TokenType::GREATER, string(1, currentChar)));
    }

    else if (currentChar == '<'){
      if (isNextChar('=')){
        const string token = string(1, currentChar) + "=";
        m_tokens.emplace_back(Token(TokenType::LESS_EQUAL, token));
        i++;
      }
      else m_tokens.emplace_back(Token(TokenType::LESS, string(1, currentChar)));
    }

    else if (currentChar == '=') {
      const string token = string(1, currentChar) + "=";
      m_tokens.emplace_back(Token(TokenType::EQUALS, token));
      i++;
    }
  }

};

//ADD NEGATIVE NUMBERS
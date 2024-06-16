#include <iostream>
#include <cstdint>
#include <vector>
#include <stdexcept>

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

      if (isNumber(currentChar)) tokenNumber(currentChar);
      
      else if (isParenthesis(currentChar)) tokenParenthesis(currentChar);

      else if (isMathOperator(currentChar)) tokenMathOperator(currentChar);

      else if (isAssignmentOperator(currentChar)) tokenAssignmentOperator(currentChar);

      else if (isComparisonOperator(currentChar)) tokenComparisonOperator(currentChar);

      else if (isIdentifier(currentChar)) tokenIdentifier(currentChar);

      else if (isSemicolon(currentChar)) tokenSemicolon(currentChar);

      else invalidToken(currentChar);
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
    if (i + 1 >= m_src.size()) return '\0';
    return m_src.at(i + 1);
  }
  
  bool isNextChar(const char& charToCheck){
    if (i + 1 >= m_src.size()) return false;
    return m_src.at(i + 1) == charToCheck;
  }

  bool isParenthesis(const char& currentChar){
    return contains("()[]{}", currentChar);
  }

  bool isDigit(const char& currentChar){
    return contains("0123456789", currentChar);
  }

  bool isNumber(const char& currentChar){
    return isDigit(currentChar) || (currentChar == '-' && isDigit(nextChar()));
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

  bool isIdentifier(const char& currentChar){
    return contains("ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz", currentChar) || isDigit(currentChar);
  }

  bool isSemicolon(const char& currentChar){
    return currentChar == ';';
  }

  void invalidToken(const char& currentChar){
    m_tokens.emplace_back(Token(TokenType::INVALID, string(1, currentChar)));
    cerr << "Invalid token detected at index: " << i << endl;
    exit(EXIT_FAILURE);
  }

  void tokenNumber(const char& currentChar){

    string number = "";
    int8_t dotCount = 0;

    if (currentChar == '-') {number += currentChar; i++;}

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

  void tokenIdentifier(const char& currentChar){
    string token = string(1, currentChar);
    while(isIdentifier(nextChar())){
      token += string(1, nextChar());
      i++;
    }
    
    if (token == "if") m_tokens.emplace_back(Token(TokenType::IF, token));
    else if (token == "else") m_tokens.emplace_back(Token(TokenType::ELSE, token));
    else if (token == "do") m_tokens.emplace_back(Token(TokenType::DO, token));
    else if (token == "while") m_tokens.emplace_back(Token(TokenType::WHILE, token));
    else if (token == "for") m_tokens.emplace_back(Token(TokenType::FOR, token));
    else if (token == "return") m_tokens.emplace_back(Token(TokenType::RETURN, token));
    else if (token == "func") m_tokens.emplace_back(Token(TokenType::FUNC, token));
    else if (token == "struct") m_tokens.emplace_back(Token(TokenType::STRUCT, token));
    else if (token == "int") m_tokens.emplace_back(Token(TokenType::INT, token));
    else if (token == "float") m_tokens.emplace_back(Token(TokenType::FLOAT, token));
    else if (token == "char") m_tokens.emplace_back(Token(TokenType::CHAR, token));
    else if (token == "string") m_tokens.emplace_back(Token(TokenType::STRING, token));
    else if (token == "bool") m_tokens.emplace_back(Token(TokenType::BOOL, token));
    else m_tokens.emplace_back(Token(TokenType::IDENTIFIER, token));
  }

  void tokenSemicolon(const char& currentChar){
    const string token = string(1, currentChar);
    m_tokens.emplace_back(Token(TokenType::SEMICOLON, token));
  }

};

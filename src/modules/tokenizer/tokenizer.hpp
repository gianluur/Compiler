#include <iostream>
#include <cstdint>
#include <vector>
#include <stdexcept>

#include "token.hpp"

#undef NULL

using std::cout, std::cerr, std::endl;
using std::string, std::vector;

class Tokenizer{
public:
  Tokenizer(string& src): m_src(src) { tokenize(); }

  vector<Token> getTokens() const { return m_tokens; }

  void tokenize(){
    for(i = 0; i < m_src.size(); i++){  
      char currentChar = m_src.at(i);

      if (isspace(currentChar)) continue;

      else if (isNumber(currentChar)) tokenNumber(currentChar);

      else if (isChar(currentChar)) tokenChar(currentChar);

      else if (isString(currentChar)) tokenString(currentChar);
      
      else if (isParenthesis(currentChar)) tokenParenthesis(currentChar);

      else if (isMathOperator(currentChar)) tokenMathOperator(currentChar);

      else if (isAssignmentOperator(currentChar)) tokenAssignmentOperator(currentChar);

      else if (isComparisonOperator(currentChar)) tokenComparisonOperator(currentChar);

      else if (isLogicalOperator(currentChar)) tokenLogicalOperator(currentChar);

      else if (isSemicolon(currentChar)) tokenSemicolon(currentChar);

      else if (isComma(currentChar)) tokenComma(currentChar);

      else  if (isDot(currentChar)) tokenDot(currentChar);

      else if (isText(currentChar)) tokenText(currentChar); 

      else invalidToken(currentChar);
    }
  }

  void print(){
    cout << "----- Tokenizer start -----" << endl;
    for(Token& token : m_tokens){
      cout << "Type: " << int(token.type) << " Lexemes: " << token.lexemes << endl;
    }
    cout << "----- Tokenizer end -----" << endl;
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
    return contains("+-*/%", currentChar) && !(nextChar() == '=');
  }

  bool isAssignmentOperator(const char& currentChar){
    return (currentChar == '=' && !isNextChar('=')) || (contains("+-*/%", currentChar) && isNextChar('='));
  }

  bool isComparisonOperator(const char& currentChar){
    return contains("<>", currentChar) || (currentChar == '=' && isNextChar('=')); 
  }

  bool isLogicalOperator(const char& currentChar){
    return contains("!&|", currentChar);
  }

  bool isText(const char& currentChar){
    return contains("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_", currentChar) || isDigit(currentChar);
  }

  bool isSemicolon(const char& currentChar){
    return currentChar == ';';
  }

  bool isComma(const char& currentChar){
    return currentChar == ',';
  }

  bool isDot(const char currentChar){
    return currentChar == '.';
  }

  bool isChar(const char& currentChar){
    return currentChar == '\'';
  }

  bool isString(const char& currentChar){
    return currentChar == '\"';
  }
  
  bool isVariable(const string& token){
    if (token == "var" ) {m_tokens.emplace_back(Token(TokenType::VAR, token)); return true;}
    else if (token == "const") {m_tokens.emplace_back(Token(TokenType::CONST, token)); return true;}
    else return false;
  }
  
  bool isType(const string& token){
    if (token == "int") {m_tokens.emplace_back(Token(TokenType::INT, token)); return true;}
    else if (token == "float") {m_tokens.emplace_back(Token(TokenType::FLOAT, token)); return true;}
    else if (token == "char") {m_tokens.emplace_back(Token(TokenType::CHAR, token)); return true;}
    else if (token == "string") {m_tokens.emplace_back(Token(TokenType::STRING, token)); return true;}
    else if (token == "bool") {m_tokens.emplace_back(Token(TokenType::BOOL, token)); return true;}
    else if (token == "null") {m_tokens.emplace_back(Token(TokenType::NULL, token)); return true;}
    else return false;
  }

  bool isIfStatement(const string& token){
    if (token == "if") {m_tokens.emplace_back(Token(TokenType::IF, token)); return true;}
    else if (token == "else") {m_tokens.emplace_back(Token(TokenType::ELSE, token)); return true;}
    else return false;
  }

  bool isLoopStatement(const string& token){
    if (token == "do") {m_tokens.emplace_back(Token(TokenType::DO, token)); return true;}
    else if (token == "while") {m_tokens.emplace_back(Token(TokenType::WHILE, token)); return true;}
    else if (token == "for") {m_tokens.emplace_back(Token(TokenType::FOR, token)); return true;}
    else if (token == "break") {m_tokens.emplace_back(Token(TokenType::BREAK, token)); return true;}
    else if (token == "continue") {m_tokens.emplace_back(Token(TokenType::CONTINUE, token)); return true;}
    else return false;
  }

  bool isFunction(const string& token){
    if (token == "func") {m_tokens.emplace_back(Token(TokenType::FUNC, token)); return true;}
    else if (token == "return") {m_tokens.emplace_back(Token(TokenType::RETURN, token)); return true;}
    else return false;
  }

  bool isStruct(const string& token){
    if (token == "struct") {m_tokens.emplace_back(Token(TokenType::STRUCT, token)); return true;}
    else return false;
  }

  bool isBoolean(const string& token){
    if (token == "true") {m_tokens.emplace_back(Token(TokenType::LITERAL_BOOLEAN, token)); return true;}
    else if (token == "false") {m_tokens.emplace_back(Token(TokenType::LITERAL_BOOLEAN, token)); return true;}
    else return false;
  }

  void invalidToken(const char& currentChar){
    m_tokens.emplace_back(Token(TokenType::INVALID, string(1, currentChar)));
    throw std::runtime_error("Invalid token detected: " + string(1, currentChar) + " index: " + std::to_string(i)); //ADD ROWS AND COLS
  }

  void tokenNumber(const char& currentChar){
    string number = "";
    int8_t dotCount = 0;

    if (currentChar == '-') {
        number += currentChar;
        i++;
    }

    for (; i < m_src.size() && (isDigit(m_src.at(i)) || (m_src.at(i) == '.')); i++) {
        if (m_src.at(i) == '.') {
            if (++dotCount > 1) {
                invalidToken(currentChar);
                break; 
            }
        }
        number += m_src.at(i);
    }
    i--; 

    if (dotCount == 0) m_tokens.emplace_back(Token(TokenType::LITERAL_INTEGER, number));
    else m_tokens.emplace_back(Token(TokenType::LITERAL_FLOAT, number));

  }

  void tokenChar(const char& currentChar){
    string token = string(1, currentChar);
    char character = nextChar();
    i++;

    char closing = nextChar();
    if (closing != '\'') invalidToken(currentChar);
    i++;

    token += string(1, character) + string(1, closing);
    m_tokens.emplace_back(Token(TokenType::LITERAL_CHARACTER, token));
  }

  void tokenString(const char& currentChar){
    string token = string(1, currentChar);
    while(nextChar() != '\"' && i < m_src.size()){
      token += string(1, nextChar());
      i++;
    }

    char closing = nextChar();
    if (closing != '\"') invalidToken(currentChar);
    i++;

    token += string(1, closing);
    m_tokens.emplace_back(Token(TokenType::LITERAL_STRING, token));
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
        invalidToken(currentChar);
        break;
    }
  }

  void tokenMathOperator(const char& currentChar){
    const string token = string(1, currentChar);
    switch (currentChar)
    {
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
          invalidToken(currentChar);
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

  void tokenLogicalOperator(const char& currentChar){
    string token = string(1, currentChar);
    while(isLogicalOperator(nextChar())){
      token += string(1, nextChar());
      i++;
    }

    if (token == "&&") m_tokens.emplace_back(Token(TokenType::AND, token));
    else if (token == "||") m_tokens.emplace_back(Token(TokenType::OR, token));
    else if (token == "!") m_tokens.emplace_back(Token(TokenType::NOT, token));
    else invalidToken(currentChar);

  }

  string getText(const char& currentChar){
    string token = string(1, currentChar);
    while(isText(nextChar())){
      token += string(1, nextChar());
      i++;
    }
    return token;
  }

  void tokenSemicolon(const char& currentChar){
    const string token = string(1, currentChar);
    m_tokens.emplace_back(Token(TokenType::SEMICOLON, token));
  }

  void tokenComma(const char& currentChar){
    const string token = string(1, currentChar);
    m_tokens.emplace_back(Token(TokenType::COMMA, token));
  }

  void tokenDot(const char& currentChar){
    const string token = string(1, currentChar);
    m_tokens.emplace_back(Token(TokenType::DOT, token));
  }

  void tokenIdentifier(const string& token){
    m_tokens.emplace_back(Token(TokenType::IDENTIFIER, token));
  }

  bool isKeyword(const string& token){
    return isVariable(token) || isType(token) || isIfStatement(token) || isLoopStatement(token) || isFunction(token) || isStruct(token) || isBoolean(token);
  }

  void tokenText(const char& currentChar){
    const string& token = getText(currentChar);
    if (!isKeyword(token)) tokenIdentifier(token);
  }
};

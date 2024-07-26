#include <iostream>
#include <cstdint>
#include <vector>
#include <stdexcept>

#include "token.hpp"
#include "error.hpp"

#undef NULL

using std::cout, std::cerr;
using std::string, std::vector;

class Tokenizer{
public:
  Tokenizer(const string& src): m_src(src), i(0), line(1) { 
    cout << "----- Tokenizer -----\n\n";
    
    tokenize(); 

    cout << "\n---------------------\n\n";
  }

  vector<Token> getTokens() const { return m_tokens; }

  void tokenize(){
    for(i = 0; i < m_src.size(); i++){  
      char currentChar = m_src.at(i);

      if (isSpace(currentChar)) continue;

      else if (isEndLine(currentChar)) { line += 1;  continue; }

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
    m_tokens.emplace_back(Token(TokenType::EOF, "\0", line));

    print();
  }

  void print() const {
    for(const Token& token : m_tokens){
      cout << "< Type: " << int(token.type) << " Lexemes: " << token.lexemes << " Line: " << token.line << " >\n" ;
    }
  }

private:
  vector<Token> m_tokens;
  string m_src;
  size_t i;
  size_t line;

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

  bool isEndLine(const char& currentChar){
    return currentChar == '\n';
  }

  bool isSpace(const char& currentChar){
    return currentChar == ' ';
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
    return contains("<>", currentChar) || (contains("!=", currentChar) && isNextChar('=')); 
  }

  bool isLogicalOperator(const char& currentChar){
    return contains("!&|", currentChar) || ((currentChar == '!') && !isNextChar('='));
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
    if (token == "var" ) {m_tokens.emplace_back(Token(TokenType::VAR, token, line)); return true;}
    else if (token == "const") {m_tokens.emplace_back(Token(TokenType::CONST, token, line)); return true;}
    else return false;
  }
  
  bool isType(const string& token){
    if (token == "int") {m_tokens.emplace_back(Token(TokenType::INT, token, line)); return true;}
    else if (token == "float") {m_tokens.emplace_back(Token(TokenType::FLOAT, token, line)); return true;}
    else if (token == "char") {m_tokens.emplace_back(Token(TokenType::CHAR, token, line)); return true;}
    else if (token == "string") {m_tokens.emplace_back(Token(TokenType::STRING, token, line)); return true;}
    else if (token == "bool") {m_tokens.emplace_back(Token(TokenType::BOOL, token, line)); return true;}
    else if (token == "null") {m_tokens.emplace_back(Token(TokenType::NULL, token, line)); return true;}
    else return false;
  }

  bool isIfStatement(const string& token){
    if (token == "if") {m_tokens.emplace_back(Token(TokenType::IF, token, line)); return true;}
    else if (token == "else") {m_tokens.emplace_back(Token(TokenType::ELSE, token, line)); return true;}
    else return false;
  }

  bool isLoopStatement(const string& token){
    if (token == "do") {m_tokens.emplace_back(Token(TokenType::DO, token, line)); return true;}
    else if (token == "while") {m_tokens.emplace_back(Token(TokenType::WHILE, token, line)); return true;}
    else if (token == "for") {m_tokens.emplace_back(Token(TokenType::FOR, token, line)); return true;}
    else if (token == "break") {m_tokens.emplace_back(Token(TokenType::BREAK, token, line)); return true;}
    else if (token == "continue") {m_tokens.emplace_back(Token(TokenType::CONTINUE, token, line)); return true;}
    else return false;
  }

  bool isFunction(const string& token){
    if (token == "func") {m_tokens.emplace_back(Token(TokenType::FUNC, token, line)); return true;}
    else if (token == "return") {m_tokens.emplace_back(Token(TokenType::RETURN, token, line)); return true;}
    else return false;
  }

  bool isStruct(const string& token){
    if (token == "struct") {m_tokens.emplace_back(Token(TokenType::STRUCT, token, line)); return true;}
    else return false;
  }

  bool isBoolean(const string& token){
    if (token == "true") {m_tokens.emplace_back(Token(TokenType::LITERAL_BOOLEAN, token, line)); return true;}
    else if (token == "false") {m_tokens.emplace_back(Token(TokenType::LITERAL_BOOLEAN, token, line)); return true;}
    else return false;
  }

  void invalidToken(const char& currentChar){
    m_tokens.emplace_back(Token(TokenType::INVALID, string(1, currentChar), line));
    error("Error: Invalid token detected: " + string(1, currentChar) + " index: " + std::to_string(i));
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

    if (dotCount == 0) m_tokens.emplace_back(Token(TokenType::LITERAL_INTEGER, number, line));
    else m_tokens.emplace_back(Token(TokenType::LITERAL_FLOAT, number, line));

  }

  void tokenChar(const char& currentChar){
    string token = string(1, currentChar);
    char character = nextChar();
    i++;

    char closing = nextChar();
    if (closing != '\'') invalidToken(currentChar);
    i++;

    token += string(1, character) + string(1, closing);
    m_tokens.emplace_back(Token(TokenType::LITERAL_CHARACTER, token, line));
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
    m_tokens.emplace_back(Token(TokenType::LITERAL_STRING, token, line));
  }

  void tokenParenthesis(const char& currentChar){
    const string token = string(1, currentChar);

    switch (currentChar){
      case '(':
        m_tokens.emplace_back(Token(TokenType::LPAREN, token, line));
        break;

      case ')':
        m_tokens.emplace_back(Token(TokenType::RPAREN, token, line));
        break;

      case '[':
        m_tokens.emplace_back(Token(TokenType::LBRACKET, token, line));
        break;

      case ']':
        m_tokens.emplace_back(Token(TokenType::RBRACKET, token, line));
        break;

      case '{':
        m_tokens.emplace_back(Token(TokenType::LCURLY, token, line));
        break;

      case '}':
        m_tokens.emplace_back(Token(TokenType::RCURLY, token, line));
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
      m_tokens.emplace_back(Token(TokenType::ADDITION, token, line));
      break;

    case '-':
      m_tokens.emplace_back(Token(TokenType::SUBTRACTION, token, line));
      break;

    case '*':
      m_tokens.emplace_back(Token(TokenType::MULTIPLICATION, token, line));
      break;

    case '/':
      m_tokens.emplace_back(Token(TokenType::DIVISION, token, line));
      break;

    case '%':
      m_tokens.emplace_back(Token(TokenType::MODULUS, token, line));
      break;

    default:
      break;
    }


  }

  void tokenAssignmentOperator(const char& currentChar){
    
    if (currentChar == '=') {
      const string token = string(1, currentChar);
      m_tokens.emplace_back(Token(TokenType::ASSIGNMENT, token, line)); 
    }

    else {
      const string token = string(1, currentChar) + "=";

      switch (currentChar){
        case '+':
          m_tokens.emplace_back(Token(TokenType::ADDITION_ASSIGNMENT, token, line));
          break;

        case '-':
          m_tokens.emplace_back(Token(TokenType::SUBTRACTION_ASSIGNMENT, token, line));
          break;
        
        case '*':
          m_tokens.emplace_back(Token(TokenType::MULTIPLICATION_ASSIGNMENT, token, line));
          break;

        case '/':
          m_tokens.emplace_back(Token(TokenType::DIVISION_ASSIGNMENT, token, line));
          break;

        case '%':
          m_tokens.emplace_back(Token(TokenType::MODULUS_ASSIGNMENT, token, line));
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
        m_tokens.emplace_back(Token(TokenType::GREATER_EQUAL, token, line));
        i++;
      }
      else m_tokens.emplace_back(Token(TokenType::GREATER, string(1, currentChar), line));
    }

    else if (currentChar == '<'){
      if (isNextChar('=')){
        const string token = string(1, currentChar) + "=";
        m_tokens.emplace_back(Token(TokenType::LESS_EQUAL, token, line));
        i++;
      }
      else m_tokens.emplace_back(Token(TokenType::LESS, string(1, currentChar), line));
    }

    else if (currentChar == '=') {
      const string token = string(1, currentChar) + "=";
      m_tokens.emplace_back(Token(TokenType::EQUALS, token, line));
      i++;
    }

    else if (currentChar == '!'){
      const string token = string(1, currentChar) + "=";
      m_tokens.emplace_back(Token(TokenType::NOT_EQUAL, token, line));
      i++;
    }
  }

  void tokenLogicalOperator(const char& currentChar){
    string token = string(1, currentChar);
    while(isLogicalOperator(nextChar())){
      token += string(1, nextChar());
      i++;
    }

    if (token == "&&") m_tokens.emplace_back(Token(TokenType::AND, token, line));
    else if (token == "||") m_tokens.emplace_back(Token(TokenType::OR, token, line));
    else if (token == "!") m_tokens.emplace_back(Token(TokenType::NOT, token, line));
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
    m_tokens.emplace_back(Token(TokenType::SEMICOLON, token, line));
  }

  void tokenComma(const char& currentChar){
    const string token = string(1, currentChar);
    m_tokens.emplace_back(Token(TokenType::COMMA, token, line));
  }

  void tokenDot(const char& currentChar){
    const string token = string(1, currentChar);
    m_tokens.emplace_back(Token(TokenType::DOT, token, line));
  }

  void tokenIdentifier(const string& token){
    m_tokens.emplace_back(Token(TokenType::IDENTIFIER, token, line));
  }

  bool isKeyword(const string& token){
    return isVariable(token) || isType(token) || isIfStatement(token) || isLoopStatement(token) || isFunction(token) || isStruct(token) || isBoolean(token);
  }

  void tokenText(const char& currentChar){
    const string& token = getText(currentChar);
    if (!isKeyword(token)) tokenIdentifier(token);
  }
};

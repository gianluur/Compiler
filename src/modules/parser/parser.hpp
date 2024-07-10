#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>

#include "../tokenizer/token.hpp"
#include "../ast/ast.hpp"

using std::cout, std::endl;
using std::stoi, std::stod;
using std::vector, std::unordered_map, std::stack;

class Parser {
public:
  Parser(vector<Token> tokens) : m_tokens(std::move(tokens)), i(0) { }

  void parse() {
    cout << "----- Parser Start -----" << endl;
    while (i < m_tokens.size()){
      Token& token = m_tokens.at(i);
      // if (token.type == TokenType::VAR || token.type == TokenType::CONST) {
      //   parseVariable();
      
      // }
      // else error("Couldn't parse the current token: " + token.lexemes);
      parseExpression();
      
    }
    cout << "----- Parser End -----" << endl << endl;

    cout << "----- AST Start -----" << endl;
    for (auto& node : m_ast) {
      node->print();
    }
    cout << "----- AST End -----" << endl << endl;
    
  }

private:
  vector<unique_ptr<ASTNode>> m_ast;
  vector<Token> m_tokens;
  size_t i;

  void error(const string& message) {
    cout << message << endl;
    exit(1);
  }

  Token& consumeToken() {
    if (i >= m_tokens.size()) error("Unexpected end of input. Last token was: " + m_tokens.at(i - 1).lexemes);
    return m_tokens.at(i++);
  }

  Token& nextToken() {
    if (i >= m_tokens.size()) error("Unexpected end of input. Last token was: " + m_tokens.at(i - 1).lexemes);
    return m_tokens.at(i);
  }

  void parseVariable() {
    Token& keyword = consumeToken();
    
    Token& type = consumeToken();
    if (!isType(type)) error("Expected type after " + keyword.lexemes + ".");

    Token& identifier = consumeToken();
    if (identifier.type != TokenType::IDENTIFIER) error("Expected type after " + keyword.lexemes + ".");

    if (nextToken().type == TokenType::ASSIGNMENT) {
      Token& assignment = consumeToken();

      Token& value = consumeToken();
      if (!isLiteral(value)) error("In this variable declaration: '" + keyword.lexemes + " " + identifier.lexemes + assignment.lexemes + "'; was expected a literal or an expression.");
 

      Token& semicolon = consumeToken();
      if (semicolon.type != TokenType::SEMICOLON) error("In this variable decleration: '" + keyword.lexemes + " " + identifier.lexemes + assignment.lexemes + value.lexemes + "'; was expected a semicolon.");
      
      if (!isMatchingType(type, value)) {
        string value_type = getLiteralType(value);
        error("In this variable decleration: '" + keyword.lexemes + " " + type.lexemes + " " + identifier.lexemes + " " + assignment.lexemes + " " + value.lexemes + semicolon.lexemes + "'; Type and value doens't match.\n Type is: " + type.lexemes + " and value type is: " + value_type);
      }
      
      m_ast.emplace_back(make_unique<Variable>(keyword, type, identifier, assignment, value, semicolon));
    }

    else if (nextToken().type == TokenType::SEMICOLON) {
      Token& semicolon = consumeToken();
      m_ast.emplace_back(make_unique<Variable>(keyword, type, identifier, semicolon));
    }

    else error("In this variable declaration: '" + keyword.lexemes + " " + identifier.lexemes + "'; was expected an assignment or a semicolon.");
  } 

  void parseExpression() {
    unordered_map<char, int> precedence = {{'+', 1}, {'-', 1}, {'*', 2}, {'/', 2}, {'%', 2}};

    stack<Token> operators;
    vector<Token> output;

    while (isValidExpressionToken()) {
        Token& current = consumeToken();

        if (current.type == TokenType::LITERAL_INTEGER || current.type == TokenType::LITERAL_FLOAT) {
            output.push_back(current);
        } 
        else if (isMathOperator(current)) {
            while (!operators.empty() && operators.top().type != TokenType::LPAREN &&
                   precedence[operators.top().lexemes[0]] >= precedence[current.lexemes[0]]) {
                output.push_back(operators.top());
                operators.pop();
            }
            operators.push(current);
        } 
        else if (current.type == TokenType::LPAREN) {
            operators.push(current);
        } 
        else if (current.type == TokenType::RPAREN) {
            while (!operators.empty() && operators.top().type != TokenType::LPAREN) {
                output.push_back(operators.top());
                operators.pop();
            }
            if (!operators.empty() && operators.top().type == TokenType::LPAREN) {
                operators.pop(); // Pop the left parenthesis
            } else {
                error("Mismatched parentheses");
            }
        }
    }

    while (!operators.empty()) {
        if (operators.top().type == TokenType::LPAREN || operators.top().type == TokenType::RPAREN) {
            error("Mismatched parentheses");
        }
        output.push_back(operators.top());
        operators.pop();
    }

    cout << "Postfix expression: ";
    for (auto& token : output) {
        cout << token.lexemes << " ";
    }
    cout << endl;

    expressionToNode(output);
  } 

  void expressionToNode(const vector<Token>& postfixExpression) {
    stack<unique_ptr<Expression>> nodes;

    for (const Token& token : postfixExpression) {
      if (token.type == TokenType::LITERAL_INTEGER) {
          nodes.push(make_unique<Integer>(token));
      } 
      else if (token.type == TokenType::LITERAL_FLOAT) {
          nodes.push(make_unique<Float>(token));
      }
      else if (isMathOperator(token)) {
        auto right = std::move(nodes.top()); nodes.pop();
        auto left = std::move(nodes.top()); nodes.pop();
        nodes.push(make_unique<BinaryOperator>(std::move(left), token, std::move(right)));
      }
    }
    m_ast.emplace_back(std::move(nodes.top()));
  }

  bool isMathOperator(const Token& token) {
    return token.lexemes == "+" || token.lexemes == "-" || 
           token.lexemes == "*" || token.lexemes == "/" || token.lexemes == "%";
  }

  bool isValidExpressionToken() {
    return i < m_tokens.size() && (nextToken().type == TokenType::LITERAL_INTEGER || 
          nextToken().type == TokenType::LITERAL_FLOAT || 
          isMathOperator(nextToken()) ||
          nextToken().type == TokenType::LPAREN || 
          nextToken().type == TokenType::RPAREN);
  }

  bool isType(const Token& token) const {
    return token.type == TokenType::INT || 
           token.type == TokenType::FLOAT ||
           token.type == TokenType::CHAR || 
           token.type == TokenType::STRING ||
           token.type == TokenType::BOOL;
  }

  bool isLiteral(const Token& token) const {
    return token.type == TokenType::LITERAL_INTEGER || 
           token.type == TokenType::LITERAL_FLOAT ||
           token.type == TokenType::LITERAL_CHARACTER ||
           token.type == TokenType::LITERAL_STRING ||
           token.type == TokenType::LITERAL_BOOLEAN;
  }

  bool isMatchingType(const Token& type, const Token& literal) const {
    return (type.type == TokenType::INT && literal.type == TokenType::LITERAL_INTEGER) || 
           (type.type == TokenType::FLOAT && literal.type == TokenType::LITERAL_FLOAT) ||
           (type.type == TokenType::CHAR && literal.type == TokenType::LITERAL_CHARACTER) ||
           (type.type == TokenType::STRING && literal.type == TokenType::LITERAL_STRING) ||
           (type.type == TokenType::BOOL && literal.type == TokenType::LITERAL_BOOLEAN);
  }

  string getLiteralType(const Token& literal) const {
    switch (literal.type){
      case TokenType::LITERAL_INTEGER: return "int";
      case TokenType::LITERAL_FLOAT: return "float";
      case TokenType::LITERAL_CHARACTER: return "char";
      case TokenType::LITERAL_STRING: return "string";
      case TokenType::LITERAL_BOOLEAN: return "bool";
      default: return "unknown";
    }
  }
};
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
      if (token.type == TokenType::VAR || token.type == TokenType::CONST) {
        parseVariable();
      
      }
      else error("Couldn't parse the current token: " + token.lexemes);
      
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

      unique_ptr<Expression> value = parseExpression();
      //if (!isLiteral(value)) error("In this variable declaration: '" + keyword.lexemes + " " + identifier.lexemes + assignment.lexemes + "'; was expected a literal or an expression.");
 

      Token& semicolon = consumeToken();
      if (semicolon.type != TokenType::SEMICOLON) error("In this variable decleration: '" + keyword.lexemes + " " + identifier.lexemes  + "'; was expected a semicolon.");
      
      m_ast.emplace_back(make_unique<Variable>(keyword, type, identifier, assignment, std::move(value), semicolon));
    }

    else if (nextToken().type == TokenType::SEMICOLON) {
      Token& semicolon = consumeToken();
      m_ast.emplace_back(make_unique<Variable>(keyword, type, identifier, semicolon));
    }

    else error("In this variable declaration: '" + keyword.lexemes + " " + identifier.lexemes + "'; was expected an assignment or a semicolon.");
  } 

   unique_ptr<Expression> parseExpression() {
    unordered_map<string, int> precedence = {{"+", 1}, {"-", 1}, {"*", 2}, {"/", 2}, {"%", 2}, {"!", 3}, {"&&", 2}, {"||", 1}};

    stack<Token> operators;
    vector<Token> output;

    while (isValidExpressionToken()) {
        Token& current = consumeToken();

        if (current.type == TokenType::LITERAL_INTEGER || current.type == TokenType::LITERAL_FLOAT || current.type == TokenType::LITERAL_BOOLEAN 
            || current.type == TokenType::LITERAL_CHARACTER || current.type == TokenType::LITERAL_STRING || current.type == TokenType::IDENTIFIER) {
            output.push_back(current);
        } 
        else if (isMathOperator(current) || isBooleanOperator(current)) {
            while (!operators.empty() && operators.top().type != TokenType::LPAREN &&
                   precedence[operators.top().lexemes] >= precedence[current.lexemes]) {
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

        else error("Unexpected token: " + current.lexemes);
    }

    while (!operators.empty()) {
        if (operators.top().type == TokenType::LPAREN || operators.top().type == TokenType::RPAREN) {
            error("Mismatched parentheses");
        }
        output.push_back(operators.top());
        operators.pop();
    }

    return expressionToNode(output);
  } 

   unique_ptr<Expression> expressionToNode(const vector<Token>& postfixExpression) {
    stack<unique_ptr<Expression>> nodes;

    for (const Token& token : postfixExpression) {
      if (token.type == TokenType::LITERAL_INTEGER) {
          nodes.push(make_unique<Integer>(token));
      } 
      else if (token.type == TokenType::LITERAL_FLOAT) {
          nodes.push(make_unique<Float>(token));
      }
      else if (token.type == TokenType::LITERAL_BOOLEAN) {
          nodes.push(make_unique<Boolean>(token));
      }
      else if (token.type == TokenType::LITERAL_CHARACTER) {
          nodes.push(make_unique<Char>(token));
      }
      else if (token.type == TokenType::LITERAL_STRING) {
          nodes.push(make_unique<String>(token));
      }
      else if (token.type == TokenType::IDENTIFIER) {
          nodes.push(make_unique<Identifier>(token));
      }

      else if (isMathOperator(token) || isBooleanOperator(token)) {
        if (token.type == TokenType::NOT){
          auto operand = std::move(nodes.top()); nodes.pop();
          nodes.push(make_unique<UnaryOperator>(token, std::move(operand)));
        }
        else {
          auto right = std::move(nodes.top()); nodes.pop();
          auto left = std::move(nodes.top()); nodes.pop();
          nodes.push(make_unique<BinaryOperator>(std::move(left), token, std::move(right)));
        }

      }
    }
    return std::move(nodes.top());
  }

  bool isBooleanOperator(const Token& token){
    return token.lexemes == "!" || token.lexemes == "&&" || token.lexemes == "||";
  }

  bool isMathOperator(const Token& token) {
    return token.lexemes == "+" || token.lexemes == "-" || 
           token.lexemes == "*" || token.lexemes == "/" || 
           token.lexemes == "%";
  }

  bool isValidExpressionToken() {
    return i < m_tokens.size() && 
          (nextToken().type == TokenType::LITERAL_INTEGER || 
          nextToken().type == TokenType::LITERAL_FLOAT || 
          nextToken().type == TokenType::LITERAL_BOOLEAN ||
          nextToken().type == TokenType::LITERAL_CHARACTER || 
          nextToken().type == TokenType::LITERAL_STRING || 
          nextToken().type == TokenType::IDENTIFIER ||
          isMathOperator(nextToken()) ||
          isBooleanOperator(nextToken()) ||
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
};
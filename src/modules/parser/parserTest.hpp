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
      parseExpression();
    }
    cout << "----- Parser End -----" << endl << endl;
    
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

unique_ptr<Expression> parseExpression() {
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

    return expressionToNode(output);
  } 

  unique_ptr<Expression> expressionToNode(const vector<Token>& postfixExpression) {
    stack<unique_ptr<Expression>> nodes;

    for (const Token& token : postfixExpression) {
      if (token.type == TokenType::LITERAL_INTEGER) {
          nodes.push(make_unique<Literal>(stoi(token.lexemes)));
      } 
      else if (isMathOperator(token)) {
        auto right = std::move(nodes.top()); nodes.pop();
        auto left = std::move(nodes.top()); nodes.pop();
        nodes.push(make_unique<BinaryOperator>(std::move(left), token, std::move(right)));
      }
    }
    return std::move(nodes.top());
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

};

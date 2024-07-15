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
      unique_ptr<ASTNode> node = parseASTNode();
      m_ast.push_back(std::move(node));
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
    if (i >= m_tokens.size()) error("ConsumeToken: Unexpected end of input. Last token was: " + m_tokens.at(i - 1).lexemes);
    return m_tokens.at(i++);
  }

  Token& nextToken() {
    if (i >= m_tokens.size()) error("nextToken: Unexpected end of input. Last token was: " + m_tokens.at(i - 1).lexemes);
    return m_tokens.at(i);
  }

  unique_ptr<ASTNode> parseASTNode(){

    Token& token = nextToken();

    switch (token.type)
    {
    case TokenType::VAR:
    case TokenType::CONST:
      return parseVariable();
      break;

    case TokenType::IDENTIFIER:
      return parseIdentifier();
      break;

    case TokenType::IF:
      return parseIfStatement();
      break;

    case TokenType::WHILE:
      return parseWhileStatement();
      break;
    
    default:
      error("Couldn't parse the current token: " + token.lexemes);
      return std::move(unique_ptr<ASTNode>());
    }

  }

  unique_ptr<While> parseWhileStatement(){
    if (nextToken().type != TokenType::WHILE) error("Expected while statement");
    const Token& whileToken = consumeToken();
    unique_ptr<Expression> condition = parseExpression();
    unique_ptr<BlockStatement> block = parseBlockStatement();
    return std::move(make_unique<While>(std::move(condition), std::move(block)));

  }

  unique_ptr<IfStatement> parseIfStatement(){
    if (nextToken().type != TokenType::IF) error("Expected if statement");
    const Token& ifToken = consumeToken();
    unique_ptr<Expression> condition = parseExpression();
    unique_ptr<BlockStatement> block = parseBlockStatement();

    if (i < m_tokens.size() && nextToken().type == TokenType::ELSE) {
      const Token& elseToken = consumeToken();
      unique_ptr<BlockStatement> elseBlock = parseBlockStatement();
      return std::move(make_unique<IfStatement>(std::move(condition), std::move(block), std::move(make_unique<ElseStatement>(std::move(elseBlock)))));
    }

    return std::move(make_unique<IfStatement>(std::move(condition), std::move(block)));
  }

  unique_ptr<BlockStatement> parseBlockStatement(){
    if (nextToken().type != TokenType::LCURLY) error("Expected open bracket after if statement");
    const Token& openBracket = consumeToken();

    vector<unique_ptr<ASTNode>> statements;
    while (nextToken().type != TokenType::RCURLY){
      if (i + 1 >= m_tokens.size()) error("No closing bracket if block statement");
      statements.emplace_back(parseASTNode());
    }

    if (nextToken().type != TokenType::RCURLY) error("Expected closing bracket after if statement");
    const Token& closingBracket = consumeToken();

    return std::move(make_unique<BlockStatement>(std::move(statements)));
  }

  unique_ptr<AssigmentOperator> parseIdentifier(){
    Token& identifier = consumeToken();
    
    if (!isAssigmentOperator(nextToken())) error("Expected assignment after identifier: " + identifier.lexemes);
    Token& assignment = consumeToken();

    unique_ptr<Expression> value = parseExpression();

    if (nextToken().type != TokenType::SEMICOLON) error("In this variable decleration: '" + identifier.lexemes  + "'; was expected a semicolon.");
    Token& semicolon = consumeToken();

    return std::move(make_unique<AssigmentOperator>(identifier, assignment, std::move(value)));
  }

  unique_ptr<Variable> parseVariable() {
    Token& keyword = consumeToken();
    
    Token& type = consumeToken();
    if (!isType(type)) error("Expected type after " + keyword.lexemes + ".");

    Token& identifier = consumeToken();
    if (identifier.type != TokenType::IDENTIFIER) error("Expected type after " + keyword.lexemes + ".");

    if (nextToken().type == TokenType::ASSIGNMENT) {
      Token& assignment = consumeToken();

      unique_ptr<Expression> value = parseExpression();

      if (nextToken().type != TokenType::SEMICOLON) error("In this variable decleration: '" + keyword.lexemes + " " + identifier.lexemes  + "'; was expected a semicolon.");
      Token& semicolon = consumeToken();
      
      return std::move(make_unique<Variable>(keyword, type, identifier, assignment, std::move(value), semicolon));
    }

    else if (nextToken().type == TokenType::SEMICOLON) {
      Token& semicolon = consumeToken();
      return std::move(make_unique<Variable>(keyword, type, identifier, semicolon));
    }

    else {
      error("In this variable declaration: '" + keyword.lexemes + " " + identifier.lexemes + "'; was expected an assignment or a semicolon.");
      return std::move(unique_ptr<Variable>());
    }
  } 

   unique_ptr<Expression> parseExpression() {
    unordered_map<string, int> precedence = { {"+", 1}, {"-", 1}, {"*", 2}, {"/", 2}, {"%", 2}, 
                                              {"==", 3}, {">", 3}, {"<", 3}, {">=", 3}, {"<=", 3},
                                              {"!", 3}, {"&&", 2}, {"||", 1}
                                            };

    stack<Token> operators;
    vector<Token> output;

    while (isValidExpressionToken()) {
        Token& current = consumeToken();

        if (current.type == TokenType::LITERAL_INTEGER || current.type == TokenType::LITERAL_FLOAT || current.type == TokenType::LITERAL_BOOLEAN 
            || current.type == TokenType::LITERAL_CHARACTER || current.type == TokenType::LITERAL_STRING || current.type == TokenType::IDENTIFIER) {
            output.push_back(current);
        } 
        else if (isMathOperator(current) || isBooleanOperator(current) || isComparisonOperator(current)) {
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
      else if (isMathOperator(token) || isBooleanOperator(token) || isComparisonOperator(token)) {
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

  bool isAssigmentOperator(const Token& token){
    return token.type == TokenType::ASSIGNMENT ||
           token.type == TokenType::ADDITION_ASSIGNMENT ||
           token.type == TokenType::SUBTRACTION_ASSIGNMENT ||
           token.type == TokenType::MULTIPLICATION_ASSIGNMENT ||
           token.type == TokenType::DIVISION_ASSIGNMENT ||
           token.type == TokenType::MODULUS_ASSIGNMENT;
  }

  bool isComparisonOperator(const Token& token) {
    return token.type == TokenType::EQUALS ||
           token.type == TokenType::NOT_EQUAL ||
           token.type == TokenType::GREATER ||
           token.type == TokenType::LESS ||
           token.type == TokenType::GREATER_EQUAL ||
           token.type == TokenType::LESS_EQUAL;
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
          isComparisonOperator(nextToken()) ||
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


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
    while (i < m_tokens.size()){
      unique_ptr<ASTNode> node = parseASTNode();
      m_ast.push_back(std::move(node));
    }

    cout << "----- AST Start -----" << endl;
    for (auto& node : m_ast) {
      node->print();
    }
    cout << "\n----- AST End -----" << endl << endl;
    
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

  bool isNextTokenType(enum TokenType type){
    return nextToken().type == type;
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
          (isMathOperator(nextToken()) ||
          isBooleanOperator(nextToken()) ||
          isComparisonOperator(nextToken()) ||
          nextToken().type == TokenType::LITERAL_INTEGER || 
          nextToken().type == TokenType::LITERAL_FLOAT || 
          nextToken().type == TokenType::LITERAL_BOOLEAN ||
          nextToken().type == TokenType::LITERAL_CHARACTER || 
          nextToken().type == TokenType::LITERAL_STRING || 
          nextToken().type == TokenType::IDENTIFIER ||
          nextToken().type == TokenType::LPAREN || 
          nextToken().type == TokenType::RPAREN);
  }

  bool isType(const Token& token) const {
    return token.type == TokenType::INT || 
           token.type == TokenType::FLOAT ||
           token.type == TokenType::CHAR || 
           token.type == TokenType::STRING ||
           token.type == TokenType::BOOL ||
           token.type == TokenType::NULL;
  }

  bool isLiteral(const Token& token) const {
    return token.type == TokenType::LITERAL_INTEGER || 
           token.type == TokenType::LITERAL_FLOAT ||
           token.type == TokenType::LITERAL_CHARACTER ||
           token.type == TokenType::LITERAL_STRING ||
           token.type == TokenType::LITERAL_BOOLEAN;
  }

  unique_ptr<ASTNode> parseASTNode(){
    Token& token = nextToken();

    switch (token.type)
    {
    case TokenType::VAR:
    case TokenType::CONST:
      return parseVariable();

    case TokenType::IDENTIFIER:
      return parseIdentifier();

    case TokenType::IF:
      return parseIfStatement();

    case TokenType::WHILE:
      return parseWhileStatement();

    case TokenType::DO:
      return parseDoStatement();

    case TokenType::FOR:
      return parseForStatement();

    case TokenType::FUNC:
      return parseFunction();

    case TokenType::STRUCT:
      return parseStruct();
    
    default:
      error("Couldn't parse the current token: " + token.lexemes);
      return unique_ptr<ASTNode>();
    }

  } 

  unique_ptr<Struct> parseStruct(){
    if (!isNextTokenType(TokenType::STRUCT)) 
      error("Expected struct");
    consumeToken();

    if (!isNextTokenType(TokenType::IDENTIFIER)) 
      error("Expected identifier after struct");
    const Token& name = consumeToken();

    unique_ptr<BlockStatement> body = parseBlockStatement();

    if (!isNextTokenType(TokenType::SEMICOLON)) 
      error("Expected identifier after struct");
    consumeToken();
    
    return make_unique<Struct>(std::move(make_unique<Identifier>(name)), std::move(body));
  }

  unique_ptr<FunctionCall>parseFunctionCall(const Token& name){
    consumeToken();

    vector<unique_ptr<Identifier>> arguments = parseArguments();
    if (!isNextTokenType(TokenType::RPAREN))
      error("Expected closing parenthesis after arguments in funtion call");
    consumeToken();

    if (!isNextTokenType(TokenType::SEMICOLON))
      error("Expected semicolon after closing parenthesis in function call");
    consumeToken();

    return make_unique<FunctionCall>(std::move(make_unique<Identifier>(name)), std::move(arguments));
  }

  vector<unique_ptr<Identifier>> parseArguments(){
    vector<unique_ptr<Identifier>> arguments;
    while (isNextTokenType(TokenType::IDENTIFIER) || isNextTokenType(TokenType::COMMA)) {
      if (!isNextTokenType(TokenType::IDENTIFIER))
        error("Expected identifier in function call");
      const Token& name = consumeToken();

      arguments.emplace_back(make_unique<Identifier>(name));

      if (isNextTokenType(TokenType::COMMA)) {
        consumeToken();
        if (isNextTokenType(TokenType::RPAREN)) 
          error("Expected another argument after comma in function call");
      }
    }
    return arguments;
  }

  vector<unique_ptr<Parameter>> parseParameters() {
    vector<unique_ptr<Parameter>> parameters;

    while (isNextTokenType(TokenType::IDENTIFIER) || isType(nextToken()) || isNextTokenType(TokenType::COMMA)){
      if (!isType(nextToken())) 
        error("Expected a type in parameter");
      const Token& type = consumeToken();

      if (!isNextTokenType(TokenType::IDENTIFIER)) 
        error("Expected identifier after type in paramerter");
      const Token& name = consumeToken();

      parameters.emplace_back(std::move(make_unique<Parameter>(type, std::move(make_unique<Identifier>(name)))));

      if (isNextTokenType(TokenType::COMMA)) {
        consumeToken();
        if (isNextTokenType(TokenType::RPAREN)) 
          error("Expected another parameter after comma in function declaration");
      }
    }

    return parameters;
  }

  unique_ptr<Function> parseFunction() {
    if (!isNextTokenType(TokenType::FUNC)) 
      error("Expected function statement");
    consumeToken();

    if (!isType(nextToken())) 
      error("Expected type after function statement");
    const Token& returnType = consumeToken();

    if (!isNextTokenType(TokenType::IDENTIFIER)) 
      error("Expected identifier after func");
    const Token& name = consumeToken();

    if (!isNextTokenType(TokenType::LPAREN)) 
      error("Expected open parenethesis for parameters after func");
    consumeToken();

    vector<unique_ptr<Parameter>> parameters = parseParameters();

    if (!isNextTokenType(TokenType::RPAREN)) 
      error("Expected open parenethesis for parameters after func");
    consumeToken();

    unique_ptr<BlockStatement> body = parseBlockStatement();

    return make_unique<Function>(returnType, std::move(make_unique<Identifier>(name)), std::move(parameters), std::move(body));

  }

  unique_ptr<Do> parseDoStatement(){
    if (!isNextTokenType(TokenType::DO)) 
      error("Expected do-while statement");
    consumeToken();

    unique_ptr<BlockStatement> body = parseBlockStatement();

    if (!isNextTokenType(TokenType::WHILE)) 
      error("Expected while after do in do-while statement");
    consumeToken();

    unique_ptr<Expression> condition = parseExpression();

    if (!isNextTokenType(TokenType::SEMICOLON)) 
      error("Expected semicolon at the end of do-while statement");
    consumeToken();

    return make_unique<Do>(std::move(body), std::move(condition));
  }

  unique_ptr<While> parseWhileStatement(){
    if (!isNextTokenType(TokenType::WHILE)) 
      error("Expected while statement");
    consumeToken();

    unique_ptr<Expression> condition = parseExpression();

    unique_ptr<BlockStatement> block = parseBlockStatement();

    return make_unique<While>(std::move(condition), std::move(block));
  }

  unique_ptr<AssigmentOperator> parseAssigmentOperator(const Token& identifier){    
    Token& op = consumeToken();

    if (!isValidExpressionToken())
      error("Expected a literal/expression/identifier after assigment operator in variable initialization");
    unique_ptr<Expression> value = parseExpression();

    if (!isNextTokenType(TokenType::SEMICOLON)) 
      error("In this variable decleration: '" + identifier.lexemes  + "'; was expected a semicolon.  last token is: " + m_tokens.at(i).lexemes);
    consumeToken();

    return make_unique<AssigmentOperator>(make_unique<Identifier>(identifier), op, std::move(value));
  }

  unique_ptr<AssigmentOperator> parseAssigmentOperator(){    
    Token& identifier = consumeToken();
    
    if (!isAssigmentOperator(nextToken())) 
      error("Expected assignment operator after identifier: " + identifier.lexemes);
    Token& op = consumeToken();

    if (!isValidExpressionToken())
      error("Expected a literal/expression/identifier after assigment operator in variable initialization");
    unique_ptr<Expression> value = parseExpression();

    if (!isNextTokenType(TokenType::SEMICOLON)) 
      error("In this variable decleration: '" + identifier.lexemes  + "'; was expected a semicolon.  last token is: " + m_tokens.at(i).lexemes);
    consumeToken();

    return make_unique<AssigmentOperator>(make_unique<Identifier>(identifier), op, std::move(value));
  }


  unique_ptr<For> parseForStatement(){
    if (!isNextTokenType(TokenType::FOR)) 
      error("Expected for statement");
    consumeToken();

    unique_ptr<Variable> initialization = parseVariable();

    unique_ptr<Expression> condition = parseExpression();

    if (!isNextTokenType(TokenType::SEMICOLON)) 
      error("Expected semicolon after condition in for loop");
    consumeToken();

    unique_ptr<AssigmentOperator> update = parseAssigmentOperator();

    unique_ptr<BlockStatement> block = parseBlockStatement();  

    return make_unique<For>(std::move(initialization), std::move(condition), std::move(update), std::move(block));
  }

  unique_ptr<IfStatement> parseIfStatement(){
    consumeToken();

    unique_ptr<Expression> condition = parseExpression();

    unique_ptr<BlockStatement> body = parseBlockStatement();

    if (i < m_tokens.size() && isNextTokenType(TokenType::ELSE)) {
      consumeToken();

      unique_ptr<BlockStatement> elseBody = parseBlockStatement();

      return make_unique<IfStatement>(std::move(condition), std::move(body), std::move(make_unique<ElseStatement>(std::move(elseBody))));
    }

    return make_unique<IfStatement>(std::move(condition), std::move(body));
  }

  unique_ptr<BlockStatement> parseBlockStatement(){
    if (!isNextTokenType(TokenType::LCURLY)) 
      error("Expected open bracket after if statement");
    consumeToken();

    vector<unique_ptr<ASTNode>> statements;
    while (!isNextTokenType(TokenType::RCURLY)){
      if (i + 1 >= m_tokens.size()) 
        error("No closing bracket if block statement");
      statements.emplace_back(parseASTNode());
    }

    if (!isNextTokenType(TokenType::RCURLY)) 
      error("Expected closing bracket after if statement");
    consumeToken();

    return make_unique<BlockStatement>(std::move(statements));
  }

  unique_ptr<ASTNode> parseIdentifier(){
    Token& identifier = consumeToken();
    if (isAssigmentOperator(nextToken())){
      return parseAssigmentOperator(identifier);
    }
    else if (isNextTokenType(TokenType::LPAREN)){
      return parseFunctionCall(identifier);
    }
    else {
      error("Expected assigment or function call after identifier" + identifier.lexemes);
      return unique_ptr<ASTNode>();
    }
  }

  unique_ptr<Variable> parseVariable() {
    Token& keyword = consumeToken();
    
    if (i >= m_tokens.size() || !isType(nextToken())) 
      error("Expected type after variable declaration"); 
    
    else if (isNextTokenType(TokenType::NULL))
      error("Null is not a valid type for a variable");

    Token& type = consumeToken();

    if (i >= m_tokens.size() || !isNextTokenType(TokenType::IDENTIFIER)) 
      error("Expected identifier after variable declaration");
    Token& identifier = consumeToken();

    if (isNextTokenType(TokenType::SEMICOLON)){
      consumeToken();

      return make_unique<Variable>(keyword, type, identifier);
    }

    else if (isNextTokenType(TokenType::ASSIGNMENT)){
      consumeToken();

      if (!isValidExpressionToken())
        error("Expected a literal/expression/identifier after assigment operator in variable initialization");
        
      unique_ptr<Expression> value = parseExpression();

      if (i >= m_tokens.size() || !isNextTokenType(TokenType::SEMICOLON))
        error("In this variable decleration: '" + keyword.lexemes + " " + identifier.lexemes  + "'; was expected a semicolon.");
      consumeToken();

      return make_unique<Variable>(keyword, type, identifier, std::move(value));
    }

    else {
      error("Expected semicolon after identifier in variable declaration");
      return unique_ptr<Variable>();
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
};


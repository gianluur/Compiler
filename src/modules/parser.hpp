#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <stack>

#include "token.hpp"
#include "ast.hpp"
#include "error.hpp"
#include "semantics.hpp"

using std::cout, std::endl;
using std::stoi, std::stod;
using std::vector, std::unordered_map, std::stack;

class Parser {
public:
  Parser(vector<Token> tokens) : m_tokens(std::move(tokens)), i(0), m_line(1), m_semantics(make_unique<Semantics>(m_line)) {
    cout << "----- AST Start -----\n";
    parse(); 
    cout << "\n-------------------\n\n";
  }

  void parse() {
    while (i < m_tokens.size()){
      if (m_tokens.at(i).type == TokenType::EOF) break;

      unique_ptr<ASTNode> node = parseASTNode();
      m_ast.push_back(std::move(node));
    }
    print();
  }

  const vector<unique_ptr<ASTNode>>& getAST() const{
    return m_ast;
  }

private:
  vector<Token> m_tokens;
  size_t i;
  size_t m_line;
  unique_ptr<Semantics> m_semantics;
  vector<unique_ptr<ASTNode>> m_ast;


  void print() const {
    for (const auto& node : m_ast) {
      node->print();
    }
  }

  Token& consumeToken() {
    if (i >= m_tokens.size()) 
      error("ConsumeToken: Unexpected end of input. Last token was: " + m_tokens.at(i - 1).lexemes, m_line);
    Token& token = m_tokens.at(i++);
    m_line = token.line - 1;
    return token;
  }

  Token& nextToken() {
    if (i >= m_tokens.size()) 
      error("nextToken: Unexpected end of input. Last token was: " + m_tokens.at(i - 1).lexemes, m_line);
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
    return token.type == TokenType::EQUALS || token.type == TokenType::NOT_EQUAL ||
           token.type == TokenType::GREATER || token.type == TokenType::LESS ||
           token.type == TokenType::GREATER_EQUAL || token.type == TokenType::LESS_EQUAL;
  }

  bool isBooleanOperator(const Token& token){
    return token.lexemes == "!" || token.lexemes == "&&" || token.lexemes == "||";
  }

  bool isMathOperator(const Token& token) {
    return token.lexemes == "+" || token.lexemes == "-" || 
           token.lexemes == "*" || token.lexemes == "/" || token.lexemes == "%";
  }

bool isValidExpressionToken() {
  if (i >= m_tokens.size()) return false;

  Token token = nextToken();
  return isMathOperator(token) || isBooleanOperator(token) || isComparisonOperator(token) || 
         token.type == TokenType::LITERAL_INTEGER || token.type == TokenType::LITERAL_FLOAT || 
         token.type == TokenType::LITERAL_BOOLEAN || token.type == TokenType::LITERAL_CHARACTER || 
         token.type == TokenType::LITERAL_STRING  || token.type == TokenType::IDENTIFIER || 
         token.type == TokenType::LPAREN || token.type == TokenType::RPAREN;
}

bool isType(const Token& token) const {
    return token.type == TokenType::INT  || 
           token.type == TokenType::INT8 || token.type == TokenType::INT16 || 
           token.type == TokenType::INT32 || token.type == TokenType::INT64 || 
           token.type == TokenType::UINT8 || token.type == TokenType::UINT16 || 
           token.type == TokenType::UINT32 || token.type == TokenType::UINT64 || 

           token.type == TokenType::FLOAT || 
           token.type == TokenType::FLOAT32 || token.type == TokenType::FLOAT64 ||
           
           token.type == TokenType::CHAR || token.type == TokenType::STRING ||
           token.type == TokenType::BOOL || token.type == TokenType::NULL;
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

    case TokenType::RETURN:
      return parseReturn();

    default:
      error("Couldn't parse the current token: " + token.lexemes, m_line);
      return unique_ptr<ASTNode>();
    }
  } 

  unique_ptr<Struct> parseStruct(){
    consumeToken();

    if (!isNextTokenType(TokenType::IDENTIFIER)) 
      error("Expected identifier after struct", m_line);
    const Token& name = consumeToken();

    unique_ptr<BlockStatement> body = parseBlockStatement();

    if (!isNextTokenType(TokenType::SEMICOLON)) 
      error("Expected semicolon after struct", m_line);
    consumeToken();
    
    // TODO add semantic check for structs;
    return make_unique<Struct>(make_unique<Identifier>(name), std::move(body));
  }

  unique_ptr<FunctionCall>parseFunctionCall(const Token& name){
    consumeToken();

    vector<unique_ptr<Expression>> arguments = parseArguments();
    
    if (!isNextTokenType(TokenType::RPAREN))
      error("Expected closing parenthesis after arguments in funtion call", m_line);
    consumeToken();

    if (!isNextTokenType(TokenType::SEMICOLON))
      error("Expected semicolon after closing parenthesis in function call", m_line);
    consumeToken();

    auto functionCall = make_unique<FunctionCall>(make_unique<Identifier>(name), std::move(arguments));
    m_semantics->analyzeFunctionCall(functionCall.get());
    return functionCall;
  }

  vector<unique_ptr<Expression>> parseArguments(){
    vector<unique_ptr<Expression>> arguments;
    if (isNextTokenType(TokenType::RPAREN)) {
      return arguments;
    }

    while (isValidExpressionToken() || isNextTokenType(TokenType::COMMA)) {
      if (!isValidExpressionToken() && !isType(nextToken()))
        error("Expected argument in function call", m_line);
      unique_ptr<Expression> argument = parseExpression();

      arguments.emplace_back(std::move(argument));

      if (isNextTokenType(TokenType::COMMA)) {
        consumeToken();
        if (isNextTokenType(TokenType::RPAREN)) 
          error("Expected another argument after comma in function call", m_line);
      }
    }
    i--;
    return arguments;
  }

  vector<unique_ptr<Parameter>> parseParameters() {
    vector<unique_ptr<Parameter>> parameters;

    while (isNextTokenType(TokenType::IDENTIFIER) || isType(nextToken()) || isNextTokenType(TokenType::COMMA)){
      if (!isType(nextToken())) 
        error("Expected a type in parameter", m_line);
      const Token& type = consumeToken();

      if (!isNextTokenType(TokenType::IDENTIFIER)) 
        error("Expected identifier after type in paramerter", m_line);
      const Token& name = consumeToken();

      parameters.emplace_back(make_unique<Parameter>(type, make_unique<Identifier>(name)));

      if (isNextTokenType(TokenType::COMMA)) {
        consumeToken();
        if (isNextTokenType(TokenType::RPAREN)) 
          error("Expected another parameter after comma in function declaration", m_line);
      }
    }

    return parameters;
  }

  unique_ptr<Return> parseReturn(){
    consumeToken();
    if (!isValidExpressionToken() && !isType(nextToken()))
      error("Expected an expresion/identifier/literal after return keyword", m_line);
    unique_ptr<Expression> value = parseExpression();
    if (!isNextTokenType(TokenType::SEMICOLON))
      error("Expected a semicolon after expression in return statement", m_line);
    consumeToken();

    return make_unique<Return>(std::move(value));
  }

  unique_ptr<Function> parseFunction() {
    consumeToken();

    if (!isType(nextToken())) 
      error("Expected type after function statement", m_line);
    const Token& returnType = consumeToken();

    if (!isNextTokenType(TokenType::IDENTIFIER)) 
      error("Expected identifier after func", m_line);
    const Token& name = consumeToken();

    if (!isNextTokenType(TokenType::LPAREN)) 
      error("Expected open parenethesis for parameters after func", m_line);
    consumeToken();

    vector<unique_ptr<Parameter>> parameters = parseParameters();

    if (!isNextTokenType(TokenType::RPAREN)) 
      error("Expected open parenethesis for parameters after func", m_line);
    consumeToken();

    unique_ptr<BlockStatement> body = parseBlockStatement();

    auto function = make_unique<Function>(returnType, make_unique<Identifier>(name), std::move(parameters), std::move(body));
    m_semantics->analyzeFunction(function.get());
    return function;

  }

  unique_ptr<Do> parseDoStatement(){
    consumeToken();

    unique_ptr<BlockStatement> body = parseBlockStatement();

    if (!isNextTokenType(TokenType::WHILE)) 
      error("Expected while after do in do-while statement", m_line);
    consumeToken();

    if (!isValidExpressionToken() && !isType(nextToken()))
      error("Expected a condition after while keyword in do-while statement", m_line);
    unique_ptr<Expression> condition = parseExpression();

    if (!isNextTokenType(TokenType::SEMICOLON)) 
      error("Expected semicolon at the end of do-while statement", m_line);
    consumeToken();

    auto doStatement =  make_unique<Do>(std::move(body), std::move(condition));
    m_semantics->analyzeConditionalStatement(doStatement.get());
    return doStatement;
  }

  unique_ptr<While> parseWhileStatement(){
    consumeToken();

    if(!isValidExpressionToken() && !isType(nextToken()))
      error("Expected condition after while keyword", m_line);
    unique_ptr<Expression> condition = parseExpression();

    unique_ptr<BlockStatement> block = parseBlockStatement();

    auto whileStatement = make_unique<While>(std::move(condition), std::move(block));
    m_semantics->analyzeConditionalStatement(whileStatement.get());
    return whileStatement;
  }

  unique_ptr<AssigmentOperator> parseAssigmentOperator(std::optional<Token> optIdentifier = {}){    

    //Getting the identifier if present
    Token identifier;
    if (optIdentifier) {
      identifier = optIdentifier.value();
    } 
    else {
      if (isNextTokenType(TokenType::IDENTIFIER))
        identifier = consumeToken();
    }
    
    //add a check here
    Token& op = consumeToken();

    if (!isValidExpressionToken() && !isType(nextToken()))
      error("Expected a literal/expression/identifier after assigment operator in variable initialization", m_line);
    unique_ptr<Expression> value = parseExpression();

    if (!isNextTokenType(TokenType::SEMICOLON)) 
      error("In this variable decleration: '" + identifier.lexemes  + "' was expected a semicolon", m_line);
    consumeToken();

    auto assigmentOperator = make_unique<AssigmentOperator>(make_unique<Identifier>(identifier), op, std::move(value));
    m_semantics->analyzeAssignmentOperator(assigmentOperator.get());
    return assigmentOperator;
  }

  unique_ptr<For> parseForStatement(){
    consumeToken();

    if (!isNextTokenType(TokenType::VAR) && !isNextTokenType(TokenType::CONST))
      error("Expected variable initialization after for keyword", m_line);
    unique_ptr<Variable> initialization = parseVariable();
    
    if (!isValidExpressionToken() && !isType(nextToken()))
      error("Expected a condition after variable initialization in for statement", m_line);
    unique_ptr<Expression> condition = parseExpression();

    if (!isNextTokenType(TokenType::SEMICOLON)) 
      error("Expected semicolon after condition in for loop", m_line);
    consumeToken();

    if (!isAssigmentOperator(nextToken()) && !isValidExpressionToken())
      error("Expected assigment operator after condition in for statement", m_line);
    unique_ptr<AssigmentOperator> update = parseAssigmentOperator();

    unique_ptr<BlockStatement> block = parseBlockStatement();  

    auto forStatement = make_unique<For>(std::move(initialization), std::move(condition), std::move(update), std::move(block));
    m_semantics->analyzeFor(forStatement.get());
    return forStatement;
  }

  unique_ptr<IfStatement> parseIfStatement(){
    consumeToken();
    
    if(!isValidExpressionToken() && !isType(nextToken()))
      error("Expected condition after if keyword", m_line);
    unique_ptr<Expression> condition = parseExpression();

    unique_ptr<BlockStatement> body = parseBlockStatement();

    if (i < m_tokens.size() && isNextTokenType(TokenType::ELSE)) {
      consumeToken();

      unique_ptr<BlockStatement> elseBody = parseBlockStatement();

      auto ifStatement = make_unique<IfStatement>(std::move(condition), std::move(body), make_unique<ElseStatement>(std::move(elseBody)));
      m_semantics->analyzeConditionalStatement(ifStatement.get());
      return ifStatement;
    }

    auto ifStatement = make_unique<IfStatement>(std::move(condition), std::move(body));
    m_semantics->analyzeIfStatement(ifStatement.get());
    return ifStatement;
  }

  unique_ptr<BlockStatement> parseBlockStatement(){
    if (!isNextTokenType(TokenType::LCURLY)) 
      error("Expected open bracket for block statement", m_line);
    consumeToken();

    vector<unique_ptr<ASTNode>> statements;
    while (!isNextTokenType(TokenType::RCURLY)){
      if (i + 1 >= m_tokens.size()) 
        error("No closing bracket in block statement", m_line);
      statements.emplace_back(parseASTNode());
    }

    if (!isNextTokenType(TokenType::RCURLY)) 
      error("Expected closing bracket after if statement", m_line);
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
      error("Expected assigment or function call after identifier" + identifier.lexemes, m_line);
      return unique_ptr<ASTNode>();
    }
  }

  unique_ptr<Variable> parseVariable() {
    Token& keyword = consumeToken();
    
    if (i >= m_tokens.size() || !isType(nextToken())) 
      error("Expected type after " + keyword.lexemes + " keyword.", m_line); 
    
    else if (isNextTokenType(TokenType::NULL))
      error("Null is not a valid type for a variable", m_line);

    Token& type = consumeToken();

    if (i >= m_tokens.size() || !isNextTokenType(TokenType::IDENTIFIER)) 
      error("Expected identifier after type in variable declaration", m_line);
    Token& name = consumeToken();

    if (isNextTokenType(TokenType::SEMICOLON)){
      consumeToken();

      auto variable = make_unique<Variable>(keyword, type, make_unique<Identifier>(name));
      m_semantics->analyzeVariable(variable.get());
      return variable;
    }

    else if (isNextTokenType(TokenType::ASSIGNMENT)){
      consumeToken();

      if (!isValidExpressionToken() && !isType(nextToken()))
        error("Error: Expected a literal/expression/identifier after assigment operator in variable initialization", m_line);
      unique_ptr<Expression> value = parseExpression();

      if (i >= m_tokens.size() || !isNextTokenType(TokenType::SEMICOLON))
        error("In this variable decleration: '" + keyword.lexemes + " " + name.lexemes  + "'; was expected a semicolon.", m_line);
      consumeToken();

      auto variable = make_unique<Variable>(keyword, type, make_unique<Identifier>(name), std::move(value));
      m_semantics->analyzeVariable(variable.get());
      return variable;
    }

    else {
      error("Expected semicolon after identifier in variable declaration", m_line);
      return unique_ptr<Variable>();
    }
  } 

   unique_ptr<Expression> parseExpression() {
    static unordered_map<string, int> precedence = { 
        {"+", 1}, {"-", 1}, {"*", 2}, {"/", 2}, {"%", 2}, 
        {"==", 3}, {">", 3}, {"<", 3}, {">=", 3}, {"<=", 3},
        {"!", 3}, {"&&", 2}, {"||", 1}
    };
    
    string castType = "";

    if (isType(nextToken())){
      const Token& typeToken = consumeToken();

      if (typeToken.type == TokenType::NULL)
        error("Can't type cast a value to null");

      if (!isNextTokenType(TokenType::LPAREN))
        error("Expected a opening parenthesis after initializing a cast");
      consumeToken();

      castType = typeToken.lexemes;
    }

    stack<Token> operators;
    vector<Token> output;

    while (isValidExpressionToken()) {
      Token& current = consumeToken();
      
      if (isLiteral(current) || current.type == TokenType::IDENTIFIER) {
        output.push_back(current);
      } 
      else if (isMathOperator(current) || isBooleanOperator(current) || isComparisonOperator(current)) {
        handleOperator(current, operators, output, precedence);
      } 
      else if (current.type == TokenType::LPAREN) {
        operators.push(current);
      } 
      else if (current.type == TokenType::RPAREN) {
        handleClosingParenthesis(operators, output);
      }
      else 
        error("Unexpected token: " + current.lexemes, m_line);
    }

    while (!operators.empty()) {
        if (operators.top().type == TokenType::LPAREN || operators.top().type == TokenType::RPAREN) 
          error("Mismatched parentheses", m_line);
        
        output.push_back(operators.top());
        operators.pop();
    }

    if (output.size() == 0)
      error("Invalid expression", m_line);

    return expressionToNode(output, castType);
  } 

  void handleClosingParenthesis(stack<Token>& operators, vector<Token>& output) {
    while (!operators.empty() && operators.top().type != TokenType::LPAREN) {
        output.push_back(operators.top());
        operators.pop();
    }
    
    if (!operators.empty() && operators.top().type == TokenType::LPAREN) {
        operators.pop(); // Pop the left parenthesis
    } 
    else {
        if (!isValidExpressionToken()) return;
        error("Mismatched parentheses", m_line);
    }
  }

  void handleOperator(const Token& current, stack<Token>& operators, vector<Token>& output, unordered_map<string, int> precedence) {
    while (!operators.empty() && operators.top().type != TokenType::LPAREN &&
           precedence.at(operators.top().lexemes) >= precedence.at(current.lexemes)) {
        output.push_back(operators.top());
        operators.pop();
    }
    operators.push(current);
  }

   unique_ptr<Expression> expressionToNode(const vector<Token>& postfixExpression, const string& castType) {
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
    
    if (castType != ""){
      i--;
      if (!isNextTokenType(TokenType::RPAREN)){
        cout << "Next: " << nextToken().lexemes;
        error("Expected closing parenthesis after the expression in cast expression");
      }
      consumeToken();
      return make_unique<Cast>(std::move(nodes.top()), castType);
    }
    else
      return std::move(nodes.top());
  }
};

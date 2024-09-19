#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <stack>

#include "../includes/token.hpp"
#include "../includes/ast.hpp"
#include "../includes/error.hpp"
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

  bool isUserDefinedType(const Token& token) const {
    const Symbol userType = m_semantics->m_scopes->find(token.lexemes);
    if (!(userType.type == "struct"))
      return false;
    return true; 
  }

  bool isLiteral(const Token& token) const {
    return token.type == TokenType::LITERAL_INTEGER || 
           token.type == TokenType::LITERAL_FLOAT ||
           token.type == TokenType::LITERAL_CHARACTER ||
           token.type == TokenType::LITERAL_STRING ||
           token.type == TokenType::LITERAL_BOOLEAN;
  }

  bool isValidExpressionToken() {
    if (i >= m_tokens.size()) return false;

    Token token = nextToken();
    return isMathOperator(token) || isBooleanOperator(token) || isComparisonOperator(token) || 
          isLiteral(token) || isType(token) || token.type == TokenType::IDENTIFIER || 
          token.type == TokenType::LPAREN || token.type == TokenType::RPAREN;
  }

  unique_ptr<ASTNode> parseASTNode(const bool isScoped = false, const string functionType = ""){
    Token& token = nextToken();

    switch (token.type){
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
      return parseReturn(isScoped, functionType);

    case TokenType::BREAK:
    case TokenType::CONTINUE:
      return parseJumps(isScoped); //break and continue

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

    if (!isNextTokenType(TokenType::LCURLY))
      error("Expected left parenthesis after name in struct declaration", m_line);
    consumeToken();

    vector<unique_ptr<Variable>> members = {};
    if (isNextTokenType(TokenType::RCURLY))
      error("Struct must contain atleast 1 member", m_line);
      
    else {
      while (i < m_tokens.size() && !isNextTokenType(TokenType::RCURLY)){
        members.emplace_back(parseVariable(true));

        if (i >= m_tokens.size() && !isNextTokenType(TokenType::RCURLY))
          error("Expected closing parenthesis after struct declaration");
      }

      if (!isNextTokenType(TokenType::RCURLY))
        error("Expected closing parenthesis after struct declaration", m_line);
      consumeToken();
    }

    if (!isNextTokenType(TokenType::SEMICOLON)) 
      error("Expected semicolon after struct", m_line);
    consumeToken();
    
    auto structure = make_unique<Struct>(make_unique<Identifier>(name), std::move(members));
    m_semantics->m_scopes->declare(name.lexemes, Symbol("struct", name.lexemes, structure.get()));
    return structure;
  }

  unique_ptr<FunctionCall> parseFunctionCall(const Token& name){
    consumeToken(); // consumes LPAREN
    
    vector<unique_ptr<Expression>> arguments;
    if (isNextTokenType(TokenType::RPAREN)){
      consumeToken();
    }
    else {
      arguments = parseArguments();
      
      if (!isNextTokenType(TokenType::RPAREN))
        error("Expected closing parenthesis after arguments in funtion call " + name.lexemes + " " + m_tokens.at(i).lexemes, m_line);
      consumeToken();
    }
  
    auto functionCall = make_unique<FunctionCall>(make_unique<Identifier>(name), std::move(arguments));
    m_semantics->analyzeFunctionCall(functionCall.get());
    return functionCall;
  }

  vector<unique_ptr<Expression>> parseArguments(){
    vector<unique_ptr<Expression>> arguments;

    while (isValidExpressionToken() || isNextTokenType(TokenType::COMMA)) {
      if (!isValidExpressionToken() && !isType(nextToken()))
        error("Expected a valid argument in function call", m_line);

      arguments.emplace_back(parseExpression(true));

      if (isNextTokenType(TokenType::COMMA)) {
        consumeToken();
        if (isNextTokenType(TokenType::RPAREN)) 
          error("Expected another argument after comma in function call", m_line);
      }
      else if (isNextTokenType(TokenType::RPAREN))
        break;
      else{
        error("Unexpected error in function call", m_line);
      }
    }
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

  unique_ptr<Return> parseReturn(const bool isScoped, const string functionType){
    const string keyword = (consumeToken()).lexemes;

    if (!isValidExpressionToken() && !isType(nextToken()))
      error("Expected an expresion/identifier/literal after return keyword", m_line);
    unique_ptr<Expression> value = parseExpression();
    if (!isNextTokenType(TokenType::SEMICOLON))
      error("Expected a semicolon after expression in return statement", m_line);
    consumeToken();

    m_semantics->isNodeScoped(keyword, isScoped);
    m_semantics->analyzeReturn(value.get(), functionType);
    return make_unique<Return>(std::move(value));
  }

  unique_ptr<Function> parseFunction() {
    consumeToken();

    if (!isType(nextToken()) && isUserDefinedType(nextToken())) 
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

    unique_ptr<BlockStatement> body = parseBlockStatement(true, returnType.lexemes, parameters);

    auto function = make_unique<Function>(returnType, make_unique<Identifier>(name), std::move(parameters), std::move(body));
    m_semantics->analyzeFunction(function.get());
    return function;
  }

  unique_ptr<LoopJumps> parseJumps(const bool isScoped) {
    const string keyword = consumeToken().lexemes;

    if (!isNextTokenType(TokenType::SEMICOLON))
      error("Missing semicolon after " + keyword);
    consumeToken();
    
    m_semantics->isNodeScoped(keyword, isScoped);
    return make_unique<LoopJumps>(keyword);
  }

  unique_ptr<Do> parseDoStatement(){
    consumeToken();

    unique_ptr<BlockStatement> body = parseBlockStatement(true);

    if (!isNextTokenType(TokenType::WHILE)) 
      error("Expected while after do in do-while statement", m_line);
    consumeToken();

    if (!isValidExpressionToken() && !isType(nextToken()))
      error("Expected a condition after while keyword in do-while statement", m_line);
    unique_ptr<Expression> condition = parseExpression();
    m_semantics->analyzeCondition(condition.get());

    if (!isNextTokenType(TokenType::SEMICOLON)) 
      error("Expected semicolon at the end of do-while statement", m_line);
    consumeToken();

    return make_unique<Do>(std::move(body), std::move(condition));
  }

  unique_ptr<While> parseWhileStatement(){
    consumeToken();

    if(!isValidExpressionToken() && !isType(nextToken()))
      error("Expected condition after while keyword", m_line);
    unique_ptr<Expression> condition = parseExpression();
    m_semantics->analyzeCondition(condition.get());

    unique_ptr<BlockStatement> block = parseBlockStatement(true);

    return make_unique<While>(std::move(condition), std::move(block));
  }

  unique_ptr<AssigmentOperator> parseAssigmentOperator(std::optional<Token> optIdentifier = {}, const bool isIdentifierMember = false){    
    //Getting the identifier if present
    Token identifier;
    if (optIdentifier) {
      identifier = optIdentifier.value();
    } 
    else {
      if (isNextTokenType(TokenType::IDENTIFIER))
        identifier = consumeToken();
    }

    if (!isAssigmentOperator(nextToken()))
      error("Expected a assigment operator after identifier", m_line);
    Token& op = consumeToken();

    if (!isValidExpressionToken() && !isType(nextToken()))
      error("Expected a literal/expression/identifier after assigment operator in variable initialization", m_line);
    unique_ptr<Expression> value = parseExpression();

    if (!isNextTokenType(TokenType::SEMICOLON)) 
      error("In this variable decleration: '" + identifier.lexemes  + "' was expected a semicolon", m_line);
    consumeToken();
    
    if (!isIdentifierMember)
      m_semantics->analyzeAssignmentOperator(identifier.lexemes, value.get());
    return make_unique<AssigmentOperator>(make_unique<Identifier>(identifier), op, std::move(value));
  }

  unique_ptr<For> parseForStatement(){
    m_semantics->m_scopes->enterScope();
    consumeToken();
    
    if (!isNextTokenType(TokenType::VAR) && !isNextTokenType(TokenType::CONST))
      error("Expected variable initialization after for keyword", m_line);
    unique_ptr<Variable> initialization = parseVariable();
    
    if (!isValidExpressionToken() && !isType(nextToken()))
      error("Expected a condition after variable initialization in for statement", m_line);
    unique_ptr<Expression> condition = parseExpression();
    m_semantics->analyzeCondition(condition.get());

    if (!isNextTokenType(TokenType::SEMICOLON)) 
      error("Expected semicolon after condition in for loop", m_line);
    consumeToken();

    if (!isAssigmentOperator(nextToken()) && !isValidExpressionToken())
      error("Expected assigment operator after condition in for statement", m_line);
    unique_ptr<AssigmentOperator> update = parseAssigmentOperator();

    unique_ptr<BlockStatement> block = parseBlockStatement(true);  

    m_semantics->m_scopes->exitScope();
    return make_unique<For>(std::move(initialization), std::move(condition), std::move(update), std::move(block));
  }

  unique_ptr<ElseStatement> parseElseStatement(){
    unique_ptr<BlockStatement>body = parseBlockStatement();
    return make_unique<ElseStatement>(std::move(body));
  }

  unique_ptr<ElseIfStatement> parseElseIfStatement(){
    consumeToken(); // consumes if token

    if(!isValidExpressionToken() && !isType(nextToken()))
      error("Expected condition after if keyword", m_line);
    unique_ptr<Expression> condition = parseExpression();
    m_semantics->analyzeCondition(condition.get());

    unique_ptr<BlockStatement> body = parseBlockStatement();
    return make_unique<ElseIfStatement>(std::move(condition), std::move(body));
  }

  unique_ptr<IfStatement> parseIfStatement(){
    consumeToken();
    
    if(!isValidExpressionToken() && !isType(nextToken()))
      error("Expected condition after if keyword", m_line);
    unique_ptr<Expression> condition = parseExpression();
    m_semantics->analyzeCondition(condition.get());

    unique_ptr<BlockStatement> body = parseBlockStatement();
    vector<unique_ptr<ElseIfStatement>> elseIfStatements = {};
    unique_ptr<ElseStatement> elseStatement = nullptr;
    
    while (isNextTokenType(TokenType::ELSE)) {
      consumeToken();
      
      if (isNextTokenType(TokenType::IF)){
        elseIfStatements.push_back(parseElseIfStatement());
      }
      else {
        elseStatement = parseElseStatement();
        break; // can only be one 1 else statement
      }
    }
    return make_unique<IfStatement>(std::move(condition), std::move(body), std::move(elseIfStatements), std::move(elseStatement));
  }

  unique_ptr<BlockStatement> parseBlockStatement(const bool isScoped = false, const string functionType = "", const vector<unique_ptr<Parameter>>& parameters = {}){
    m_semantics->m_scopes->enterScope();

    if (!isNextTokenType(TokenType::LCURLY)) 
      error("Expected open bracket for block statement", m_line);
    consumeToken();

    if (parameters.size() != 0){
      for (const unique_ptr<Parameter>& parameter: parameters){
        Parameter* current = parameter.get();
        m_semantics->m_scopes->declare(current->getName(), Symbol("var", current->getType())); //add const
      }
    }

    vector<unique_ptr<ASTNode>> statements;
    while (!isNextTokenType(TokenType::RCURLY)){
      if (i + 1 >= m_tokens.size()) 
        error("No closing bracket in block statement", m_line);
      statements.emplace_back(parseASTNode(isScoped, functionType));
    }

    if (!isNextTokenType(TokenType::RCURLY)) 
      error("Expected closing bracket after if statement", m_line);
    consumeToken();

    m_semantics->m_scopes->exitScope();
    return make_unique<BlockStatement>(std::move(statements));
  }

  unique_ptr<DotOperator> parseDotOperator(const Token& identifier, const bool isInsideExpression = false) {
    consumeToken(); //consumes the dot

    if (!isNextTokenType(TokenType::IDENTIFIER))
      error("Expected a identifier after the dot operator", m_line);
    const Token& member = consumeToken();

    if (isAssigmentOperator(nextToken())){

      if (isInsideExpression)
        error("Assignment operations aren't possible within an expression", m_line);
      unique_ptr<AssigmentOperator> assignment = parseAssigmentOperator(member, true);
      
      m_semantics->analyzeDotOperator(identifier.lexemes, member.lexemes, assignment.get());
      return make_unique<DotOperator>(make_unique<Identifier>(identifier), make_unique<Identifier>(member), std::move(assignment));
    }
    else {
      m_semantics->analyzeDotOperator(identifier.lexemes, member.lexemes, nullptr);
      return make_unique<DotOperator>(make_unique<Identifier>(identifier), make_unique<Identifier>(member), nullptr);
    } 
  };

  //THIS FUNCTION HAS A WHOLE CAN BE IMPROVED A LOT
  unique_ptr<ASTNode> parseIdentifier(){ 
    const Token& identifier = consumeToken();
    if (isAssigmentOperator(nextToken())){
      return parseAssigmentOperator(identifier);
    }
    else if (isNextTokenType(TokenType::LPAREN)){
      //THIS HAS TO BE IMPROVED IN STYLE

      unique_ptr<FunctionCall> functionCall = parseFunctionCall(identifier);

      if (!isNextTokenType(TokenType::SEMICOLON))
        error("Expected semicolon after closing parenthesis in function call", m_line);
      consumeToken();
      return std::move(functionCall);

    }
    else if (isNextTokenType(TokenType::DOT)){
      return parseDotOperator(identifier);
    }
    else {
      error("Expected assigment or function call after identifier" + identifier.lexemes, m_line);
      return unique_ptr<ASTNode>();
    }
  }
  
  unique_ptr<Variable> parseVariable(const bool isMember = false) {
    Token& keyword = consumeToken();
    
    if (i >= m_tokens.size() || (!isType(nextToken()) && isUserDefinedType(nextToken()))) 
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
      m_semantics->analyzeVariable(variable.get(), isMember);
      return variable;
    }

    else if (isNextTokenType(TokenType::ASSIGNMENT)){
      consumeToken();

      if (isType(type)){ 
        if (!isValidExpressionToken() && !isType(nextToken()))
          error("Error: Expected a literal/expression/identifier after assigment operator in variable initialization", m_line);
        unique_ptr<Expression> value = parseExpression();

        if (i >= m_tokens.size() || !isNextTokenType(TokenType::SEMICOLON))
          error("In this variable decleration: '" + keyword.lexemes + " " + name.lexemes  + "'; was expected a semicolon.", m_line);
        consumeToken();

        auto variable = make_unique<Variable>(keyword, type, make_unique<Identifier>(name), std::move(value));
        m_semantics->analyzeVariable(variable.get(), isMember);
        return variable;
      }
      //the type is struct so it must have an initializer
      else {
        if (!isNextTokenType(TokenType::LCURLY)){
          error("Expected initializer in struct variable '" + name.lexemes + "'", m_line);
          return nullptr;
        }
        consumeToken();

        vector<unique_ptr<Expression>> members;
        while (isValidExpressionToken() || isNextTokenType(TokenType::COMMA)) {
          if (!isValidExpressionToken() && !isType(nextToken())){
            error("Expected a valid struct member to initialize it", m_line);
            return nullptr;
          }
          members.emplace_back(parseExpression());

          cout << nextToken().lexemes << '\n';

          if (isNextTokenType(TokenType::COMMA)) {
            cout << "Test\n";
            consumeToken();
            if (isNextTokenType(TokenType::RCURLY)){
              error("Expected another argument after comma in struct initializer", m_line);
              return nullptr;
            }
          }
          else if (isNextTokenType(TokenType::RCURLY)){
            consumeToken();
            break;
          }
          else {
            error("Unexpected error in variable declaration", m_line);
            return nullptr;
          }
        }
        
        if (i >= m_tokens.size() || !isNextTokenType(TokenType::SEMICOLON))
          error("In this variable decleration: '" + keyword.lexemes + " " + name.lexemes  + "'; was expected a semicolon.", m_line);
        consumeToken();

        auto variable = make_unique<Variable>(keyword, type, make_unique<Identifier>(name), make_unique<StructInitialization>(std::move(members)));
        m_semantics->analyzeVariable(variable.get(), isMember);
        return variable;
      }
    }

    else {
      error("Expected semicolon after identifier in variable declaration", m_line);
      return nullptr;
    }
  } 

  unique_ptr<Cast> parseCastExpression(const Token& type){
    if (!isNextTokenType(TokenType::LPAREN))
      error("Expected opening parenthesis after type in a cast expression", m_line);
    consumeToken();

    unique_ptr<Expression> expression = parseExpression(true);

    if (!isNextTokenType(TokenType::RPAREN))
      error("Expected closing parenthesis after the expression to cast", m_line);
    consumeToken();

    auto cast =  make_unique<Cast>(std::move(expression), type.lexemes);
    m_semantics->analyzeCast(cast.get());
    return cast;
  }

  unique_ptr<Expression> parseExpression(const bool isWrappedInParenthesis = false){
    static unordered_map<string, int> precedence = { 
      {"+", 1}, {"-", 1}, {"*", 2}, {"/", 2}, {"%", 2}, 
      {"==", 3}, {">", 3}, {"<", 3}, {">=", 3}, {"<=", 3},
      {"!", 3}, {"&&", 2}, {"||", 1}
    };

    //const string castType = checkInitialCast();

    stack<Token> operators;
    stack<unique_ptr<Expression>> operands;

    while (isValidExpressionToken()) {
        Token& current = consumeToken();
        cout << current.lexemes << '\n';

        if (isType(current)){
          if (current.type == TokenType::NULL)
            error("Null is not a valid type to cast into", m_line);
          operands.push(parseCastExpression(current));
        }
        else if (isLiteral(current)) {
          operands.push(parseLiterals(current));
        } // THIS CODE SNIPPET I BELIVE CAN BE MADE INTO JUST THE PARSE IDENTIFIER CALL
        else if (current.type == TokenType::IDENTIFIER) {
          if (isNextTokenType(TokenType::LPAREN)){
            operands.push(parseFunctionCall(current));
          }
          else if (isNextTokenType(TokenType::DOT)){
            operands.push(parseDotOperator(current, true));
          }
          else {
            operands.push(make_unique<Identifier>(current));
          }
        } 
        else if (isMathOperator(current) || isBooleanOperator(current) || isComparisonOperator(current)) {
          handleOperator(current, operators, operands, precedence);
        } 
        else if (current.type == TokenType::LPAREN) {
          operators.push(current);
        } 
        else if (current.type == TokenType::RPAREN) {
          bool shouldExpressionEnd = false;
          handleClosingParenthesis(operators, operands, isWrappedInParenthesis, shouldExpressionEnd);
          if (isWrappedInParenthesis && shouldExpressionEnd){
            break;
          }
        }
        else {
          error("Unexpected token: " + current.lexemes, m_line);
        }
    }

    while (!operators.empty()) {
      if (operators.top().type == TokenType::LPAREN || operators.top().type == TokenType::RPAREN) 
        error("Mismatched parentheses", m_line);
      applyOperator(operators, operands);
    }

    if (operands.size() == 0)
      error("Invalid expression", m_line);
    auto expression = std::move(operands.top());

    return expression;
  }

  void handleClosingParenthesis(stack<Token>& operators, stack<unique_ptr<Expression>>& operands, const bool isWrappedInParenthesis, bool& shouldExpressionEnd) {
    while (!operators.empty() && operators.top().type != TokenType::LPAREN) {
      applyOperator(operators, operands);
    }
    
    if (!operators.empty() && operators.top().type == TokenType::LPAREN) {      
      operators.pop(); // Pop the left parenthesis
    } 
    else {
      if (isWrappedInParenthesis){
        shouldExpressionEnd = true;
        i--;
        return;
      }

      if (!isValidExpressionToken()) return;
      error("Mismatched parentheses " + nextToken().lexemes, m_line);
    }
  }

  unique_ptr<Literal> parseLiterals(const Token& current){
    switch(current.type) {
      case TokenType::LITERAL_INTEGER:
        return make_unique<Integer>(current);

      case TokenType::LITERAL_FLOAT:
        return make_unique<Float>(current);

      case TokenType::LITERAL_BOOLEAN:
        return make_unique<Boolean>(current);
        
      case TokenType::LITERAL_CHARACTER:
        return make_unique<Char>(current);

      case TokenType::LITERAL_STRING:
        return make_unique<String>(current);

      default:
        error("Unknown literal type in expression", current.line);
        return nullptr;
    }
  }

  string checkInitialCast(){
    if (isType(nextToken())){
      const Token& typeToken = consumeToken();

      if (typeToken.type == TokenType::NULL)
        error("Can't type cast a value to null");

      if (!isNextTokenType(TokenType::LPAREN))
        error("Expected a opening parenthesis after initializing a cast");
      consumeToken();

      return typeToken.lexemes;
    }
    return "";
  }

  void applyOperator(stack<Token>& operators, stack<unique_ptr<Expression>>& operands) {
    Token op = operators.top();
    operators.pop();

    if (op.type == TokenType::NOT) {
      auto operand = std::move(operands.top()); operands.pop();
      operands.push(make_unique<UnaryOperator>(op, std::move(operand)));
    } 
    else {
      auto right = std::move(operands.top()); operands.pop();
      auto left = std::move(operands.top()); operands.pop();
      operands.push(make_unique<BinaryOperator>(std::move(left), op, std::move(right)));
    }
  }

  void handleOperator(const Token& current, stack<Token>& operators, stack<unique_ptr<Expression>>& operands, unordered_map<string, int> precedence) {
    while (!operators.empty() && operators.top().type != TokenType::LPAREN &&
          precedence.at(operators.top().lexemes) >= precedence.at(current.lexemes)) {
      applyOperator(operators, operands);
    }
    operators.push(current);
  }
};
 
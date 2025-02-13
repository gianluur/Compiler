#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>  
#include <unordered_set>
#include <stack>
#include <optional>

#include "../includes/token.hpp"
#include "../includes/ast.h"
#include "../includes/error.hpp"
#include "scope.hpp"

using std::cout;
using std::vector, std::unordered_map, std::unordered_set, std::stack;
using std::unique_ptr, std::make_unique;
using std::optional;

#undef NULL

class Parser {
public:
  Parser(vector<Token> tokens): m_tokens(std::move(tokens)), index(0), m_line(1) {
    cout << "----- AST Start -----\n";
    parse(); 
    cout << "\n-------------------\n\n";
  }

  ~Parser(){}

  void parse() {
    while (index < m_tokens.size()){
      unique_ptr<ASTNode> node = getASTNode();
      m_ast.push_back(std::move(node));
    }
    print();
  }

  const vector<unique_ptr<ASTNode>>& getAST() const {
    return m_ast;
  }

private:
  vector<Token> m_tokens;
  vector<unique_ptr<ASTNode>> m_ast;
  size_t index;
  size_t m_line;

  const unordered_set<TokenType> assignmentOperatorSet = {
    TokenType::ASSIGNMENT,
    TokenType::ADDITION_ASSIGNMENT,
    TokenType::SUBTRACTION_ASSIGNMENT,
    TokenType::MULTIPLICATION_ASSIGNMENT,
    TokenType::DIVISION_ASSIGNMENT,
    TokenType::MODULUS_ASSIGNMENT,
  };

  const unordered_set<TokenType> comparisonOperatorSet = {
    TokenType::EQUALS,
    TokenType::NOT_EQUAL,
    TokenType::LESS,
    TokenType::GREATER,
    TokenType::LESS_EQUAL,
    TokenType::GREATER_EQUAL,
  };

  const unordered_set<TokenType> booleanOperatorSet = {
    TokenType::NOT,
    TokenType::AND,
    TokenType::OR,
  };

  const unordered_set<TokenType> mathOperatorSet = {
    TokenType::ADDITION,
    TokenType::SUBTRACTION,
    TokenType::STAR,
    TokenType::DIVISION,
    TokenType::MODULUS,
  };

  const unordered_set<TokenType> basicTypeSet = {
    TokenType::INT,
    TokenType::INT8,
    TokenType::INT16,
    TokenType::INT32,
    TokenType::INT64,
    TokenType::UINT,
    TokenType::UINT8,
    TokenType::UINT16,
    TokenType::UINT32,
    TokenType::UINT64,
    TokenType::FLOAT,
    TokenType::FLOAT32,
    TokenType::FLOAT64,
    TokenType::CHAR,
    TokenType::STRING,
    TokenType::BOOL,
    TokenType::NULL,
  };

  const unordered_set<TokenType> literalsMap = {
    TokenType::LITERAL_INTEGER,
    TokenType::LITERAL_FLOAT,
    TokenType::LITERAL_CHARACTER,
    TokenType::LITERAL_STRING,
    TokenType::LITERAL_BOOLEAN,
  };

  unique_ptr<ASTNode> getASTNode(const TokenType scope = TokenType::NULL) {
    const Token& token = nextToken();

    switch(token.type){
      case TokenType::VAR:
      case TokenType::CONST:
        return parseVariable();

      case TokenType::FUNC:
        return parseFunction();

      case TokenType::RETURN:
        return parseReturn(scope);

      case TokenType::IDENTIFIER:
      case TokenType::CARET:
        return parseIdentifier({}, false);

      case TokenType::IF:
        return parseIfStatement();

      case TokenType::BREAK:
      case TokenType::CONTINUE:
        return parseLoopControl(scope);

      case TokenType::WHILE:
        return parseWhileStatement();

      case TokenType::DO:
        return parseDoWhileStatement();

      case TokenType::FOR:
        return parseForStatement();

      case TokenType::STRUCT:
        return parseStruct();
      
      default:
        error("Token Not handled yet: " + token.lexemes, m_line);
    }
  }

  void print() const {
    for (const unique_ptr<ASTNode>& node : m_ast) {
      node->print();
    } 
  }

  const Token& consumeToken(){
    if (index >= m_tokens.size())
      error("Compiler Error: consumeToken(), in parser.hpp, has gone out of bounds");
    return m_tokens[index++];
  }

  const Token& nextToken() const {
    if (index >= m_tokens.size())
      error("Compiler Error: nextToken(), in parser.hpp, has gone out of bounds");
    return m_tokens[index];
  }

  bool isAtEnd() const {
    return index >= m_tokens.size();
  }

  bool isNextTokenType(enum TokenType type){
    return nextToken().type == type;
  }

  int getPrecedence(const TokenType type) const {
    switch (type) {
      case TokenType::NOT:
      case TokenType::AMPERSAND:
      case TokenType::CARET:
        return 4;

      case TokenType::STAR: 
      case TokenType::DIVISION:
      case TokenType::MODULUS:
        return 3;

      case TokenType::ADDITION: 
      case TokenType::SUBTRACTION:
        return 2;

      case TokenType::GREATER: 
      case TokenType::GREATER_EQUAL:
      case TokenType::LESS: 
      case TokenType::LESS_EQUAL:
      case TokenType::EQUALS:
        return 1;

      case TokenType::AND:
      case TokenType::OR:
        return 0;

      default:
        return -1;
    }
  }

  bool isLeftAssociative(const TokenType type) const {
    return type != TokenType::NOT && type != TokenType::AMPERSAND && type != TokenType::CARET; // Unary NOT is right associative
  }

  bool isAssigmentOperator(const Token& token){
    return assignmentOperatorSet.find(token.type) != assignmentOperatorSet.end();
  }

  bool isComparisonOperator(const Token& token) {
    return comparisonOperatorSet.find(token.type) != comparisonOperatorSet.end();
  }

  bool isBooleanOperator(const Token& token){
    return booleanOperatorSet.find(token.type) != booleanOperatorSet.end();
  }

  bool isMathOperator(const Token& token) {
    return mathOperatorSet.find(token.type) != mathOperatorSet.end();
  }

  bool isOperator(const Token& token){
    return isMathOperator(token) || isBooleanOperator(token) || isComparisonOperator(token) || 
           token.type == TokenType::AMPERSAND || token.type == TokenType::CARET;
  }

  bool isLiteral(const Token& token) const {
    return literalsMap.find(token.type) != literalsMap.end();
  }

  bool isType(const Token& token) const {
    return basicTypeSet.find(token.type) != basicTypeSet.end() || isStructType(token);
  }

  bool isValidExpression(const Token& token) {
    return index < m_tokens.size() &&
           !(token.type == TokenType::LCURLY || token.type == TokenType::RCURLY) &&
           (isLiteral(token) || isType(token) || isOperator(token) ||
            token.type == TokenType::IDENTIFIER || token.type == TokenType::LPAREN ||
            token.type == TokenType::RPAREN);
  }

  bool isStructType(const Token& token) const {
    return Scope::getInstance()->find(token.lexemes, false).type == ASTNodeType::STRUCTURE;
  }

  unique_ptr<Variable> parseVariable(const bool isMember = false){
    const Token& keyword = consumeToken(); // consumes the var/const token

    if (!isType(nextToken()))
      error("In variable declaration was expected a type after " + keyword.lexemes + " keyword", m_line);
    unique_ptr<Type> type = make_unique<Type>(consumeToken(), isNextTokenType(TokenType::STAR) ? true : false);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
    if (type->isPointer()) 
      consumeToken();

    if (!isNextTokenType(TokenType::IDENTIFIER))
      error("In variable declaration was expected a identifier after type: " + keyword.lexemes + " " + type->toString(), m_line);
    unique_ptr<Identifier> identifier = make_unique<Identifier>(consumeToken());

    if (isNextTokenType(TokenType::SEMICOLON)){
      consumeToken(); // consumes ';'
      return make_unique<Variable>(keyword, std::move(type), std::move(identifier), isMember);
    }
    else if (isNextTokenType(TokenType::ASSIGNMENT)){
      consumeToken(); // consumes '='

      if (isNextTokenType(TokenType::LCURLY)){
        unique_ptr<ListInitializer> listInitializer = parseListInitializer();
        
        if (!isNextTokenType(TokenType::SEMICOLON))
          error("In list initializer was expected a semicolon after the closing parenthesis", m_line);
        consumeToken();
        
        return make_unique<Variable>(keyword, std::move(type), std::move(identifier), std::move(listInitializer), isMember);
      }
      else {
        if (!isValidExpression(nextToken()))
          error("In variable declaration was expected an appropriate value after assigment operator: " + keyword.lexemes + " " + type->toString() + " " + identifier->toString(), m_line);
        unique_ptr<Expression> value = parseExpression();
        
        if (!isNextTokenType(TokenType::SEMICOLON))
          error("In variable declaration was expected a semicolon", m_line);
        consumeToken();

        return make_unique<Variable>(keyword, std::move(type), std::move(identifier), std::move(value), isMember);
      }
    }
    else
      error(std::string((isMember) ? "In struct" : "In variable") + " declaration was expected a semicolon", m_line);
  }

  unique_ptr<Function> parseFunction(){
    consumeToken();

    if (!isType(nextToken()))
      error("In function declaration was expected a type after func keyword", m_line);
    unique_ptr<Type> type = make_unique<Type>(consumeToken(), isNextTokenType(TokenType::STAR) ? true : false);
    if (type->isPointer()) 
      consumeToken();

    if (!isNextTokenType(TokenType::IDENTIFIER))
      error("In function declaration was expected a identifier after type: func " + type->toString(), m_line);
    unique_ptr<Identifier> identifier = make_unique<Identifier>(consumeToken());

    if (!isNextTokenType(TokenType::LPAREN))
      error("In function declaration was expected a opening parenthesis after identifier: func " + type->toString()  + identifier->toString(), m_line);
    consumeToken();

    vector<unique_ptr<Parameter>> parameters = {};
    while(!isNextTokenType(TokenType::RPAREN)){
      parameters.push_back(parseParameter());

      if (isNextTokenType(TokenType::COMMA)){
        consumeToken();
        if (isNextTokenType(TokenType::RPAREN))
          error("In function declaration, when passing parameters, there's a comma but no parameter after it, remove the comma or pass the parameter: func " + type->toString()  + identifier->toString());
      }
    }
    consumeToken(); //consumes the ')'
    const vector<unique_ptr<Parameter>>& paramRef = parameters;
    unique_ptr<Body> body = parseBody(TokenType::FUNC, paramRef);
    
    return make_unique<Function>(std::move(type), std::move(identifier), std::move(parameters), std::move(body));
  }

  unique_ptr<Parameter> parseParameter(){
    if (!isType(nextToken()))
      error("In function declaration was expected a type for the parameter", m_line);
    unique_ptr<Type> type = make_unique<Type>(consumeToken(), isNextTokenType(TokenType::STAR) ? true : false);
    if (type->isPointer()) 
      consumeToken();

    if (!isNextTokenType(TokenType::IDENTIFIER))
      error("In function declaration was expected a identifer after type for a parameter:" + type->toString(), m_line);
    unique_ptr<Identifier> identifier = make_unique<Identifier>(consumeToken());

    return make_unique<Parameter>(std::move(type), std::move(identifier));
  }

  unique_ptr<Body> parseBody(const TokenType scope = TokenType::NULL, const vector<unique_ptr<Parameter>>& parameters = {}) {
    Scope::getInstance()->enterScope();

    if (!parameters.empty()){
      for (const auto& parameter: parameters)
        Scope::getInstance()->declare(parameter->getIdentifier(), Symbol(parameter.get()));
    }

    if (!isNextTokenType(TokenType::LCURLY))
      error("Expected open curly bracket for the body", m_line);
    consumeToken();

    vector<unique_ptr<ASTNode>> statements = {};
    while(!isNextTokenType(TokenType::RCURLY)){
      statements.push_back(getASTNode(scope));
    }
    consumeToken(); // consumes the '}'

    Scope::getInstance()->exitScope();
    return make_unique<Body>(std::move(statements));
  }

  unique_ptr<FunctionCall> parseFunctionCall(const Token& token, const bool isInsideExpression = false){    
    consumeToken(); //TODO: ma che cazzo e'
        
    unique_ptr<Identifier> identifier = make_unique<Identifier>(token);
    vector<unique_ptr<Expression>> arguments;

    if (isNextTokenType(TokenType::RPAREN))
      consumeToken();
    else {
      do {
        arguments.push_back(parseExpression(true));

        if (isNextTokenType(TokenType::COMMA)){
          consumeToken();
          if (isNextTokenType(TokenType::RPAREN))
            error("In function call was expected another parameter after comma", m_line);
        }
      } while (!isNextTokenType(TokenType::RPAREN));

      if (!isNextTokenType(TokenType::RPAREN))
        error("In function call was expected a closing parenthesis after the arguments", m_line);
      consumeToken();
    }

    if (!isInsideExpression){
      if (!isNextTokenType(TokenType::SEMICOLON))
        error("In function call was expected a semicolon", m_line);
      consumeToken();
    }
    else {
      if (!isValidExpression(nextToken()) && !isNextTokenType(TokenType::SEMICOLON))
        error("In function call was expected a semicolon", m_line);
    }
    
    return make_unique<FunctionCall>(std::move(identifier), std::move(arguments), isInsideExpression);
  }

  unique_ptr<Return> parseReturn(const TokenType scope) {
    consumeToken();

    if (!isValidExpression(nextToken()))
      error("In return statement was expected an expression");
    unique_ptr<Expression> expression = parseExpression();

    if (!isNextTokenType(TokenType::SEMICOLON))
      error("In return statement was expected a semicolon", m_line);
    consumeToken();

    return make_unique<Return>(std::move(expression), scope);
  }

  unique_ptr<DotOperator> parseDotOperator(const Token& token, const bool isInsideExpression = false) {
    consumeToken(); // consumes '.'
    unique_ptr<Identifier> identifier = make_unique<Identifier>(token);

    if (!isNextTokenType(TokenType::IDENTIFIER))
      error("In dot operator was expected a member after the dot", m_line);
    if (!isInsideExpression){
      unique_ptr<AssignmentOperator> assignment = parseAssignmentOperator(consumeToken(), false, true);
      return make_unique<DotOperator>(std::move(identifier), std::move(assignment));
    }
    else {
      unique_ptr<Identifier> member = make_unique<Identifier>(consumeToken());
      return make_unique<DotOperator>(std::move(identifier), std::move(member));
    }
  }

  unique_ptr<ASTNode> parseIdentifier(optional<Token> token, const bool isInsideExpression = false) {
    bool isDereference;
    if ((isDereference = isNextTokenType(TokenType::CARET)))
      consumeToken();

    if (!token.has_value())
      token = consumeToken();
    
    if (isInsideExpression){
      if (isNextTokenType(TokenType::LPAREN))
        return parseFunctionCall(token.value(), isInsideExpression);
      else
        if (isNextTokenType(TokenType::DOT))
            return parseDotOperator(token.value(), isInsideExpression);
        else
          return make_unique<Identifier>(token.value());
    }
    else
      if (isNextTokenType(TokenType::DOT))
        return parseDotOperator(token.value(), isInsideExpression);
      else if (isNextTokenType(TokenType::LPAREN))
        return parseFunctionCall(token.value(), isInsideExpression);
      else
        return parseAssignmentOperator(token.value(), false, false, isDereference);
  }
                
  unique_ptr<AssignmentOperator> parseAssignmentOperator(const Token& token, const bool isInsideParenthesis = false, const bool isDotOperator = false, const bool isDereference = false) {
    unique_ptr<Identifier> identifier = make_unique<Identifier>(token);

    if (!isAssigmentOperator(nextToken()))
      error("In assignment operator was expected the operator after the identifier: " + nextToken().lexemes, m_line);
    unique_ptr<Operator> op = make_unique<Operator>(consumeToken());

    if (!isValidExpression(nextToken()))
      error("In assignment operator was expected a valid expression after the operator", m_line);
    unique_ptr<Expression> value = parseExpression();

    if (!isNextTokenType(TokenType::SEMICOLON))
      error("In assignment operator was expected a semicolon after the value", m_line);
    consumeToken();

    return make_unique<AssignmentOperator>(std::move(identifier), std::move(op), std::move(value), isDotOperator, isDereference);
  }

  unique_ptr<If> parseIfStatement(){
    consumeToken();

    if (!isNextTokenType(TokenType::LPAREN))
      error("In if statement declaration was expected a opening parenthesis before the condition", m_line);
    consumeToken();

    if (!isValidExpression(nextToken()))
      error("In if statement declaration was expected a valid expression for the condition", m_line);
    unique_ptr<Expression> condition = parseExpression(true, true);

    if (!isNextTokenType(TokenType::RPAREN))
      error("In if statement declaration was expected a closing parenthesis after the condition", m_line);
    consumeToken();

    unique_ptr<Body> body = parseBody();

    vector<unique_ptr<Else>> elses = {};
    while (!isAtEnd() && isNextTokenType(TokenType::ELSE))
        elses.push_back(parseElseStatement());
    

    return make_unique<If>(std::move(condition), std::move(body), std::move(elses));
  }

  unique_ptr<Else> parseElseStatement(){
    consumeToken(); //consumes 'else'
    
    if (isNextTokenType(TokenType::IF)){
      unique_ptr<If> ifstatement = parseIfStatement();
      return make_unique<Else>(std::move(ifstatement));
    }
    else {
      unique_ptr<Body> body = parseBody();
      return make_unique<Else>(std::move(body));
    }
  }

  unique_ptr<LoopControl> parseLoopControl(const TokenType scope){
    const Token& keyword = consumeToken();
    
    if (!isNextTokenType(TokenType::SEMICOLON))
      error("Expected semicolon after " + keyword.lexemes + "keyword", m_line);
    consumeToken();

    return make_unique<LoopControl>(keyword, scope);
  }

  unique_ptr<While> parseWhileStatement(){
    consumeToken();

    if (!isNextTokenType(TokenType::LPAREN))
      error("In while statement declaration was expected a opening parenthesis before the condition", m_line);
    consumeToken();

    if (!isValidExpression(nextToken()))
      error("In while statement declaration was expected a valid expression for the condition", m_line);
    unique_ptr<Expression> condition = parseExpression(true, true);

    if (!isNextTokenType(TokenType::RPAREN))
      error("In while statement declaration was expected a closing parenthesis after the condition", m_line);
    consumeToken();

    unique_ptr<Body> body = parseBody(TokenType::WHILE);

    return make_unique<While>(std::move(condition), std::move(body));
  }

  unique_ptr<DoWhile> parseDoWhileStatement(){
    consumeToken();

    unique_ptr<Body> body = parseBody(TokenType::DO);
    if (!isNextTokenType(TokenType::WHILE))
      error("In do-while statement declaration was expected the while token after the body");
    consumeToken();

    if (!isNextTokenType(TokenType::LPAREN))
      error("In while statement declaration was expected a opening parenthesis before the condition", m_line);
    consumeToken();

    if (!isValidExpression(nextToken()))
      error("In while statement declaration was expected a valid expression for the condition", m_line);
    unique_ptr<Expression> condition = parseExpression(true, true);

    if (!isNextTokenType(TokenType::RPAREN))
      error("In while statement declaration was expected a closing parenthesis after the condition", m_line);
    consumeToken();

    if (!isNextTokenType(TokenType::SEMICOLON))
      error("In do-while statement declaration was expected a semicolon",m_line);
    consumeToken();

    return make_unique<DoWhile>(std::move(condition), std::move(body));
  }

  unique_ptr<For> parseForStatement(){
    consumeToken(); // consumes 'for'

    if (!isNextTokenType(TokenType::LPAREN))
      error("In for statement declaration was expected a opening parenthesis before the condition", m_line);
    consumeToken();

    if (!isNextTokenType(TokenType::VAR) && !isNextTokenType(TokenType::CONST))
      error("In for statement declaration was expected a valid initialization", m_line);
    unique_ptr<Variable> initialization = parseVariable();

    if (!isValidExpression(nextToken()))
      error("In for statement decalration was expecteda a valid condition", m_line);
    unique_ptr<Expression> condition = parseExpression(false, true);

    if (!isNextTokenType(TokenType::SEMICOLON))
      error("In for statement declaration was expected a semicolon", m_line);
    consumeToken();

    if (!isNextTokenType(TokenType::IDENTIFIER))
      error("In for statement declaration was expected an identifier for the update");  
    unique_ptr<AssignmentOperator> update = parseAssignmentOperator(consumeToken(), true);

    if (!isNextTokenType(TokenType::RPAREN))
      error("In for statement declaration was expected a closing renthesis after the update", m_line);
    consumeToken();

    unique_ptr<Body> body = parseBody(TokenType::FOR);

    return make_unique<For>(std::move(initialization), std::move(condition), std::move(update), std::move(body));
  }

  unique_ptr<Struct> parseStruct() {
    consumeToken(); // consumes 'struct'

    if (!isNextTokenType(TokenType::IDENTIFIER))
      error("In struct declaration was expected an identifier after struct keyword", m_line);
    unique_ptr<Identifier> identifier = make_unique<Identifier>(consumeToken());

    if (!isNextTokenType(TokenType::LCURLY))
      error("In struct declaration was expected a opening curly bracket after the identifier: ", m_line);
    consumeToken();

    vector<unique_ptr<Variable>> members = {};
    while(!isNextTokenType(TokenType::RCURLY)){
      if (!isNextTokenType(TokenType::VAR) && !isNextTokenType(TokenType::CONST))
        error("In struct declaration members can only be declared/initialized with var/const", m_line);
      members.push_back(parseVariable(true));
    }
    consumeToken(); // consumes '}'
    if (!isNextTokenType(TokenType::SEMICOLON)) 
      error("In struct declaration was expected a semicolon after closing curly bracket", m_line);
    consumeToken(); // consumes ';'

    return make_unique<Struct>(std::move(identifier), std::move(members));
  }

  unique_ptr<ListInitializer> parseListInitializer() {
    consumeToken(); //consumes '{'

    vector<unique_ptr<Expression>> elements;
    while(!isNextTokenType(TokenType::RCURLY)){
      elements.push_back(make_unique<Expression>(parseExpression(true), false));

      if (isNextTokenType(TokenType::COMMA)){
        consumeToken();
        if (isNextTokenType(TokenType::RCURLY))
          error("In list initializer was expected another parameter after comma", m_line);
      }
    }

    if (!isNextTokenType(TokenType::RCURLY))
      error("In list initializer was expected a closing parenthesis after the arguments", m_line);
    consumeToken();

    return make_unique<ListInitializer>(std::move(elements));
  }
  
  unique_ptr<Cast> parseCast(const Token& token){
    unique_ptr<Type> type = make_unique<Type>(token, false);
    if (token.type == TokenType::NULL)
      error("Casting any type to type 'null' is not allowed", m_line);
    else if (isNextTokenType(TokenType::STAR))
      error("Casting to pointer is not allowed", m_line);

    if (!isNextTokenType(TokenType::LPAREN))
      error("In cast expression was expected an opening parenthesis", m_line);
    consumeToken();

    unique_ptr<Expression> expression = parseExpression(true);

    if (!isNextTokenType(TokenType::RPAREN))
      error("In cast expression was expected a closing parenthesis", m_line);
    consumeToken();

    return make_unique<Cast>(std::move(type), std::move(expression));
  }

  unique_ptr<Expression> parseExpression(const bool isInsideParenthesis = false, const bool isCondition = false) {
    stack<unique_ptr<Operator>> operators;
    stack<unique_ptr<ASTNode>> nodes;
    int parenCount = 0;

    while (!isAtEnd()) {
      if ((!isInsideParenthesis && isNextTokenType(TokenType::SEMICOLON)) || 
          (isInsideParenthesis && parenCount == 0 && 
          (isNextTokenType(TokenType::COMMA) || isNextTokenType(TokenType::RPAREN) || isNextTokenType(TokenType::RCURLY)))) 
        break;

      const Token& token = nextToken();

      switch (token.type) {
        case TokenType::IDENTIFIER:
          consumeToken();
          nodes.push(parseIdentifier(token, true));
          break;

        case TokenType::LPAREN:
          consumeToken();
          operators.push(make_unique<Operator>(token));
          parenCount++;
          break;

        case TokenType::RPAREN:
          consumeToken();
          while (!operators.empty() && operators.top()->getOperator() != TokenType::LPAREN) {
            handleOperator(operators, nodes);
          }
          if (!operators.empty() && operators.top()->getOperator() == TokenType::LPAREN) {
            operators.pop(); // Pop '('
            parenCount--;
          } 
          else if (parenCount > 0) {
            error("Mismatched parentheses in expression", m_line);
          }
          break;

        default:
          if (isLiteral(token)) {
            consumeToken();
            nodes.push(make_unique<Literal>(token));
          } 
          else if (isOperator(token)) {
            consumeToken();
            int currentPrecedence = getPrecedence(token.type);
            bool leftAssociative = isLeftAssociative(token.type);

            while (!operators.empty() &&
                operators.top()->getOperator() != TokenType::LPAREN &&
                ((leftAssociative && currentPrecedence <= getPrecedence(operators.top()->getOperator())) ||
                (!leftAssociative && currentPrecedence < getPrecedence(operators.top()->getOperator())))) {
              handleOperator(operators, nodes);
            }
            operators.push(make_unique<Operator>(token));
          }
          else if (isType(token)){
            consumeToken();
            nodes.push(parseCast(token));
          } 
          else 
            error("Unknown token in expression: " + token.lexemes, m_line);
      }
    }

    if (parenCount > 0)
      error("Unmatched opening parenthesis in expression", m_line);

    while (!operators.empty()) {
      if (operators.top()->getOperator() == TokenType::LPAREN) 
        error("Unmatched opening parenthesis in expression", m_line);
      handleOperator(operators, nodes);
    }

    return nodes.empty() ? nullptr : make_unique<Expression>(std::move(nodes.top()), isCondition);
  }

  void handleOperator(stack<unique_ptr<Operator>>& operators, stack<unique_ptr<ASTNode>>& nodes) {
    unique_ptr<Operator> op = std::move(operators.top());
    operators.pop();

    const TokenType operatorToken = op->getOperator();
    if (operatorToken == TokenType::NOT || operatorToken == TokenType::AMPERSAND || operatorToken == TokenType::CARET) {
      unique_ptr<ASTNode> right = std::move(nodes.top());
      nodes.pop();
      nodes.push(make_unique<UnaryOperator>(std::move(op), std::move(right)));
    } 
    else {
      unique_ptr<ASTNode> right = std::move(nodes.top());
      nodes.pop();
      unique_ptr<ASTNode> left = std::move(nodes.top());
      nodes.pop();
      nodes.push(make_unique<BinaryOperator>(std::move(left), std::move(op), std::move(right)));
    }
  }
};  
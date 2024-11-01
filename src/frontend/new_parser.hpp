#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>  
#include <unordered_set>
#include <optional>
#include <stack>

#include "../includes/token.hpp"
#include "../includes/new_ast.hpp"
#include "../includes/error.hpp"
#include "semantics.hpp"

using std::cout, std::endl;
using std::stoi, std::stod;
using std::vector, std::unordered_map, std::unordered_set, std::stack;

#undef NULL

class NEWParser {
public:
  NEWParser(vector<Token> tokens) : m_tokens(std::move(tokens)), index(0), m_line(1) {
    cout << "----- AST Start -----\n";
    parse(); 
    cout << "\n-------------------\n\n";
  }

  ~NEWParser(){}

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
    TokenType::MULTIPLICATION,
    TokenType::DIVISION,
    TokenType::MODULUS,
  };

  const unordered_set<TokenType> basicTypeSet = {
    TokenType::INT,
    TokenType::FLOAT,
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

  unique_ptr<ASTNode> getASTNode() {
    const Token& token = nextToken();

    switch(token.type){
      case TokenType::VAR:
      case TokenType::CONST:
        return parseVariable();
      
      default:
        error("Token Not handled yet: " + token.lexemes, m_line);
    }
  }

  void print() const {
    for (const auto& node : m_ast) {
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

  bool isNextTokenType(enum TokenType type){
    return nextToken().type == type;
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

  bool isLiteral(const Token& token) const {
    return literalsMap.find(token.type) != literalsMap.end();
  }

  bool isType(const Token& token) const {
    return basicTypeSet.find(token.type) != basicTypeSet.end();
  }

  bool isValidExpression(const Token& token) {
    return (index >= m_tokens.size()) && 
          (isMathOperator(token) || isBooleanOperator(token) || isComparisonOperator(token) || 
          isLiteral(token) || isType(token) || token.type == TokenType::IDENTIFIER || 
          token.type == TokenType::LPAREN || token.type == TokenType::RPAREN);
  }

  unique_ptr<Variable> parseVariable(){
    const Token& keyword = consumeToken(); // consumes the var/const token

    if (!isType(nextToken()))
      error("In variable declaration was expected a type after " + keyword.lexemes + " keyword", m_line);
    unique_ptr<Type> type = make_unique<Type>(consumeToken());

    if (!isNextTokenType(TokenType::IDENTIFIER))
      error("In variable declaration was expected a identifier after type: " + keyword.lexemes + " " + type->toString(), m_line);
    unique_ptr<Identifier> identifier = make_unique<Identifier>(consumeToken());

    if (isNextTokenType(TokenType::SEMICOLON)){
      consumeToken(); // consumes ';'
      return make_unique<Variable>(keyword, std::move(type), std::move(identifier));
    }
    // else if (isNextTokenType(TokenType::ASSIGNMENT)){
    //   consumeToken(); // consumes '='

    //   if (!isValidExpression(nextToken()))
    //     error("In variable declaration was expected an appropriate value after assigment operator: " + keyword + " " + type + " " + identifier->toString(), m_line);

    // }
    error("Variable stuff not handled yet\n");
  }



};
 
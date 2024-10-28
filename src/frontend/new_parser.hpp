#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>  
#include <unordered_set>
#include <optional>
#include <stack>

#include "../includes/token.hpp"
#include "../includes/ast.hpp"
#include "../includes/error.hpp"
#include "semantics.hpp"

using std::cout, std::endl;
using std::stoi, std::stod;
using std::vector, std::unordered_map, std::unordered_set, std::stack;

#undef NULL

class NEWParser {
public:
  NEWParser(vector<Token> tokens) : m_tokens(std::move(tokens)), index(0), m_line(1), m_semantics(make_unique<Semantics>(m_line)) {
    cout << "----- AST Start -----\n";
    parse(); 
    cout << "\n-------------------\n\n";
  }

  ~NEWParser(){}

  void parse() {

    print();
  }

  const vector<unique_ptr<ASTNode>>& getAST() const {
    return m_ast;
  }

private:
  vector<Token> m_tokens;
  size_t index;
  size_t m_line;

  unique_ptr<Semantics> m_semantics;
  vector<unique_ptr<ASTNode>> m_ast;

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

  void print() const {
    for (const auto& node : m_ast) {
      node->print();
    } 
  }

  const Token& nextToken(){
    if (index + 1 >= m_tokens.size())
      error("Compiler Error: nextToken(), in parser.hpp, has gone out of bounds");
    return m_tokens[index++];
  }

  const Token& peekNextToken() const {
    if (index + 1 >= m_tokens.size())
      error("Compiler Error: peekNextToken(), in parser.hpp, has gone out of bounds");
    return m_tokens[index + 1];
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

  void parse(){
    while (index < m_tokens.size()){
      
    }
  }
  




};
 
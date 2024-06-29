#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "../tokenizer/token.hpp"
#include "../ast/ast.hpp"

using std::cout, std::endl;
using std::stoi, std::stod;
using std::vector;

class Parser {
public:
  Parser(vector<Token> tokens) : m_tokens(tokens) { }

  void parse() {
    cout << "----- Parser Start -----" << endl;
    for(i = 0; i < m_tokens.size(); i++) {
      Token& token = m_tokens.at(i);
      unique_ptr<Number> left = parseNumber(token);
      i++;
      char op = m_tokens.at(i).lexemes.at(0);
      i++;
      unique_ptr<Number> right = parseNumber(m_tokens.at(i));
      i++;
      BinaryOperator expression(op, move(left), move(right));
    
    }
    cout << "----- Parser End -----" << endl;
  }
private:
  vector<Token> m_tokens;
  size_t i;

  unique_ptr<Number> parseNumber(Token& token) {

  if (token.type == TokenType::LITERAL_INTEGER) {
    return make_unique<Integer>(stoi(token.lexemes));
  }
  else if (token.type == TokenType::LITERAL_FLOAT) {
    return make_unique<Float>(stod(token.lexemes));
  }

  return nullptr;
}

};
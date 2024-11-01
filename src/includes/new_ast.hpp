#pragma once

#include <iostream>
#include <vector>
#include <iomanip>
#include <memory>

#include "../includes/token.hpp"

using std::cout, std::endl, std::setw;
using std::size_t, std::string, std::vector, std::unique_ptr, std::make_unique;

class ASTNode {
public:
  virtual ~ASTNode() = default;
  virtual void print(int indentation_level = 0) const = 0;
};

class Type : public ASTNode {
public:
  Type(const Token& token) : m_type(token.type), m_str(token.lexemes) {}

  void print(int indentation_level = 0) const override {
    cout << setw(indentation_level) << " " << "Type: " << m_str << '\n';
  }

  TokenType getType() const { 
    return m_type; 
  }

  bool isInteger() const {
    return m_type >= TokenType::INTEGERS_BEGIN && m_type <= TokenType::INTEGERS_END;
  }

  bool isFloat() const {
    return m_type >= TokenType::FLOAT_BEGIN && m_type <= TokenType::FLOAT_END;
  }

  bool isBool() const {
    return m_type == TokenType::BOOL;
  }

  bool isChar() const {
    return m_type == TokenType::CHAR;
  }

  bool isNull() const {
    return m_type == TokenType::NULL;
  }

  string toString() const {
    return m_str;
  }

private:
  const TokenType m_type;
  const string m_str;
};

class Literal: public ASTNode {
public:
  Literal(const Token& token) : m_type(token.type), m_str(token.lexemes) {}
  Literal(): m_type(TokenType::NULL), m_str("null") {}

  void print(int indentation_level = 0) const override {
    cout << setw(indentation_level) << " " << "Value: " << m_str << '\n';
  }
  
  TokenType getType() const { 
    return m_type; 
  }

  bool isInteger() const {
    return m_type == TokenType::LITERAL_INTEGER;
  }

  bool isFloat() const {
    return m_type == TokenType::LITERAL_FLOAT;
  }

  bool isBool() const {
    return m_type == TokenType::BOOL;
  }

  bool isChar() const {
    return m_type == TokenType::CHAR;
  }

  bool isNull() const {
    return m_type == TokenType::NULL;
  }

  string toString() const {
    return m_str;
  }

private:
  const TokenType m_type;
  const string m_str;
};

class Identifier: public ASTNode {
public:
  Identifier(const Token& token): m_str(token.lexemes) {}

  void print(int indentation_level = 0) const override {
    cout << setw(indentation_level) << " " << "Identifier: " << m_str << '\n';
  }

  string toString() const {
    return m_str;
  }

private:
  const string m_str;
};

class Expression: public ASTNode {

};

class Variable: public ASTNode {
public:
  Variable(const Token& keyword, unique_ptr<Type> type, unique_ptr<Identifier> identifier):
    m_keyword(keyword), m_type(std::move(type)), m_identifier(std::move(identifier)), m_value(make_unique<Literal>()) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Variable {" << '\n';
    cout << setw(indentation_level + 2) << " " << "keyword: " << m_keyword.lexemes << '\n';
    m_type->print(indentation_level + 2);
    m_identifier->print(indentation_level + 2);
    m_value->print(indentation_level + 2);
    cout << setw(indentation_level) << " " << "}\n";
  }

private:
  const Token& m_keyword;
  unique_ptr<Type> m_type;
  unique_ptr<Identifier> m_identifier;
  unique_ptr<Literal> m_value;
};

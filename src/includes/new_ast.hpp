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

class Operator: public ASTNode {
public:
  Operator(const Token& token): op(token.type), m_str(token.lexemes) {}

  void print(int indentation_level = 0) const override {
    cout << setw(indentation_level) << " " << "Operator: " << m_str << '\n';
  }

  TokenType getOperator() const {
    return op;
  }

private:
  const TokenType op;
  const string m_str;
};

class BinaryOperator: public ASTNode {
public:
  BinaryOperator(unique_ptr<ASTNode> left, unique_ptr<Operator> op, unique_ptr<ASTNode> right):
    m_left(std::move(left)), m_op(std::move(op)), m_right(std::move(right)) {}

  void print(int indentation_level = 0) const override {
    cout << setw(indentation_level) << " " << "Binary Operator{\n";
    m_left->print(indentation_level + 2);
    m_op->print(indentation_level + 2);
    m_right->print(indentation_level + 2);
    cout << setw(indentation_level) << " " << "}\n";
  }

  TokenType getOperator() const {
    return m_op->getOperator();
  }

private:
  unique_ptr<ASTNode> m_left;
  unique_ptr<Operator> m_op;
  unique_ptr<ASTNode> m_right;
};

class UnaryOperator: public ASTNode {
public:
  UnaryOperator(unique_ptr<Operator> op, unique_ptr<ASTNode> right):
    m_op(std::move(op)), m_right(std::move(right)) {}

  void print(int indentation_level = 0) const override {
    cout << setw(indentation_level) << " " << "Unary Operator{";
    m_op->print(indentation_level + 2);
    m_right->print(indentation_level + 2);
    cout << setw(indentation_level) << " " << "}\n";
  }

  TokenType getOperator() const {
    return m_op->getOperator();
  }

private:
  unique_ptr<Operator> m_op;
  unique_ptr<ASTNode> m_right;
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

class Expression: public ASTNode {
public:
  Expression(unique_ptr<ASTNode> start):
    m_start(std::move(start)) {}

  Expression():
    m_start(make_unique<Literal>()) {}
    

  void print(int indentation_level = 0) const override {
    cout << setw(indentation_level) << " " << "Expression {\n";
    m_start->print(indentation_level + 2);
    cout << setw(indentation_level) << " " << "}\n";
  }

  ASTNode* getExpression() const {
    return m_start.get();
  }

private:
  unique_ptr<ASTNode> m_start;
};

class Cast: public ASTNode {
public:
  Cast(unique_ptr<Type> type, unique_ptr<Expression> expression):
    m_type(std::move(type)), m_expression(std::move(expression)) {}

  void print(int indentation_level = 0) const override {
    cout << setw(indentation_level) << " " << "Cast {\n";
    m_type->print(indentation_level + 2);
    m_expression->print(indentation_level + 2);
    cout << setw(indentation_level) << " " << "}\n";
  }

  ASTNode* getExpression() const {
    return m_expression->getExpression();
  }

  TokenType getType() const {
    return m_type->getType();
  }

private:
  unique_ptr<Type> m_type;
  unique_ptr<Expression> m_expression;
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

class AssignmentOperator: public ASTNode {
public:
  AssignmentOperator(unique_ptr<Identifier> identifier, unique_ptr<Operator> op, unique_ptr<Expression> value):
    m_identifier(std::move(identifier)), m_op(std::move(op)), m_value(std::move(value)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Assignment Operator {\n";
    m_identifier->print(indentation_level + 2);
    m_op->print(indentation_level + 2);
    m_value->print(indentation_level + 2);
    cout << setw(indentation_level) << " " << "}\n";
  }

private:
  unique_ptr<Identifier> m_identifier;
   unique_ptr<Operator> m_op;
   unique_ptr<Expression> m_value;

};

class Variable: public ASTNode {
public:
  Variable(const Token& keyword, unique_ptr<Type> type, unique_ptr<Identifier> identifier):
    m_keyword(keyword), m_type(std::move(type)), m_identifier(std::move(identifier)), m_value(make_unique<Expression>()) {}
    
  Variable(const Token& keyword, unique_ptr<Type> type, unique_ptr<Identifier> identifier, unique_ptr<Expression> value):
    m_keyword(keyword), m_type(std::move(type)), m_identifier(std::move(identifier)), m_value(std::move(value)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Variable {\n";
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
  unique_ptr<Expression> m_value;
};

class FunctionCall: public ASTNode {
public:
  FunctionCall(unique_ptr<Identifier> identifier, vector<unique_ptr<Expression>> arguments):
    m_identifier(std::move(identifier)), m_arguments(std::move(arguments)) {}

  void print(int indentation_level = 0) const override {
    cout << setw(indentation_level) << " " << "Function Call{\n";
    m_identifier->print(indentation_level + 2);    
    for(const auto& argument: m_arguments){
      argument->print(indentation_level + 2);
    }
    cout << setw(indentation_level) << " " << "}\n";
  }

private:
  unique_ptr<Identifier> m_identifier;
  vector<unique_ptr<Expression>> m_arguments;

};

class Parameter: public ASTNode {
public:
  Parameter(unique_ptr<Type> type, unique_ptr<Identifier> name):
    m_type(std::move(type)), m_identifier(std::move(name)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Parameter {\n";
    m_type->print(indentation_level + 2);
    m_identifier->print(indentation_level + 2);
    cout << setw(indentation_level) << " " << "}\n";
  }

  TokenType getType() const {
    return m_type->getType();
  }

  string getIdentifier() const {
    return m_identifier->toString();
  }

private:
  unique_ptr<Type> m_type;
  unique_ptr<Identifier> m_identifier;
};

class Body: public ASTNode {
public:
  Body(vector<unique_ptr<ASTNode>> statements):
  m_statements(std::move(statements)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Body {";
    for(const auto& statement: m_statements){
      statement->print(indentation_level + 2);
    }
    cout << setw(indentation_level) << " " << "}\n";
  } 

private:
  vector<unique_ptr<ASTNode>> m_statements;
};

class Function: public ASTNode {
public:
  Function(unique_ptr<Type> type, unique_ptr<Identifier> identifier, vector<unique_ptr<Parameter>> parameters, unique_ptr<Body> body):
    m_type(std::move(type)), m_identifier(std::move(identifier)), m_parameters(std::move(parameters)), m_body(std::move(body)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Function {\n";
    m_type->print(indentation_level + 2);
    m_identifier->print(indentation_level + 2);
    for(const auto& parameter: m_parameters){
      parameter->print(indentation_level + 2);
    }
    m_body->print(indentation_level + 2);
    cout << setw(indentation_level) << " " << "}\n";
  }

  TokenType getType() const {
    return m_type->getType();
  }

  string getIdentifier() const {
    return m_identifier->toString();
  }

  vector<Parameter*> getParameter() const {
    vector<Parameter*> parameters;
    for(const auto& current: m_parameters){
      parameters.push_back(current.get());
    }
    return parameters;
  }

private:
  unique_ptr<Type> m_type;
  unique_ptr<Identifier> m_identifier;
  vector<unique_ptr<Parameter>> m_parameters;
  unique_ptr<Body> m_body;
};



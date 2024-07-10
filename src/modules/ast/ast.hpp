#include <iostream>
#include <iomanip>
#include <memory>

#include "../tokenizer/token.hpp"

using std::cout, std::endl, std::setw;
using std::unique_ptr, std::make_unique;

enum class ASTNodeType {
  INVALID,
  NUMBER,
  BINARY_OPERATOR,
};

class ASTNode {
public:
  virtual ~ASTNode() = default;
  virtual void print(int indentation_level = 0) const = 0;
};

class Expression : public ASTNode {};
class Literal : public Expression {};

class BinaryOperator : public Expression {
public:
  BinaryOperator(unique_ptr<Expression> left, const Token& op, unique_ptr<Expression> right): 
    m_left(std::move(left)), m_op(op.lexemes), m_right(std::move(right)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "BinaryOperator { " << '\n';
    cout << setw(indentation_level + 2) << " " << "operator: " << m_op << "\n";
    cout << setw(indentation_level + 2) << " " <<  "left: ";
    m_left->print(indentation_level + 1);
    cout << setw(indentation_level + 2) << " " << "right: ";
    m_right->print(indentation_level + 3);
    cout <<  setw(indentation_level) << " " << "} " << endl;
  }

private:
  unique_ptr<Expression> m_left;
  string m_op;
  unique_ptr<Expression> m_right;

};

class Integer : public Literal {
public:
  Integer(const Token& token) : m_token(token.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << "LiteralInteger: " << m_token << endl;
  }

private:
  string m_token;
};

class Float : public Literal {
public:
  Float(const Token& token) : m_token(token.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << "Literal Float: " << m_token << endl;
  }

private:
  string m_token;
};

class Variable : public ASTNode {
public:

  Variable(const Token& keyword, const Token& type, const Token& identifier, const Token& assignment, const Token& value, const Token& semicolon):
    m_keyword(keyword.lexemes), m_type(type.lexemes), m_identifier(identifier.lexemes), m_assignment(assignment.lexemes), m_value(value.lexemes), m_semicolon(semicolon.lexemes) {}
  Variable(const Token& keyword, const Token& type, const Token& identifier, const Token& semicolon):
    m_keyword(keyword.lexemes), m_type(type.lexemes), m_identifier(identifier.lexemes), m_semicolon(semicolon.lexemes) {}

  void print(int indentation_level = 0) const override {
    if (m_assignment.empty()) {
      cout << "Variable-Init: " << m_keyword << " " << m_type << " " << m_identifier << m_semicolon << endl;
    }
    else {
      cout << "Variable-Assign: " << m_keyword << " " << m_type << " " << m_identifier << " " << m_assignment << " " << m_value << m_semicolon << endl;
    }
  }

private:
  string m_keyword;
  string m_type;
  string m_identifier;
  string m_assignment;
  string m_value;
  string m_semicolon;
};
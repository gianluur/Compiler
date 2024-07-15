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

class Expression : public ASTNode {
public:
  string infix;
};

class Literal : public Expression {};

class Integer : public Literal {
public:
  Integer(const Token& token) : m_token(token.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level + 2) << " " << "LiteralInteger { " << '\n';
    cout << setw(indentation_level + 4) << " " << "value: " << m_token << "\n";
    cout <<  setw(indentation_level + 2) << " " << "} " << endl;
  }

private:
  string m_token;
};

class Float : public Literal {
public:
  Float(const Token& token) : m_token(token.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level + 2) << " " << "LiteralFloat { " << '\n';
    cout << setw(indentation_level + 4) << " " << "value: " << m_token << "\n";
    cout <<  setw(indentation_level + 2) << " " << "} " << endl;
  }

private:
  string m_token;
};

class String: public Literal {
public:
  String(const Token& token) : m_token(token.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level + 2) << " " << "LiteralString { " << '\n';
    cout << setw(indentation_level + 4) << " " << "value: " << m_token << "\n";
    cout <<  setw(indentation_level + 2) << " " << "} " << endl;
  }
private:
  string m_token;
};

class Char : public Literal {
public:
  Char(const Token& token) : m_token(token.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level + 2) << " " << "LiteralChar { " << '\n';
    cout << setw(indentation_level + 4) << " " << "value: " << m_token << "\n";
    cout <<  setw(indentation_level + 2) << " " << "} " << endl;
  }
private:
  string m_token;
};

class Boolean : public Literal {
public:
  Boolean(const Token& token) : m_token(token.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level + 2) << " " << "LiteralBoolean { " << '\n';
    cout << setw(indentation_level + 4) << " " << "value: " << m_token << "\n";
    cout <<  setw(indentation_level + 2) << " " << "} " << endl;
  }
private:
  string m_token;
};

class Identifier : public Expression {
public:
  Identifier(const Token& token) : m_token(token.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level + 2) << " " << "Identifier { " << '\n';
    cout << setw(indentation_level + 4) << " " << "name: " << m_token << "\n";
    cout <<  setw(indentation_level + 2) << " " << "} " << endl;
  }
private:
  string m_token;
};

class UnaryOperator: public Expression {
public:
  UnaryOperator(const Token& op, unique_ptr<Expression> operand):
    m_op(op.lexemes), m_operand(std::move(operand)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "UnaryOperator { " << '\n';
    cout << setw(indentation_level + 2) << " " << "operator: " << m_op << "\n";
    cout << setw(indentation_level + 2) << " " << "operand: ";
    m_operand->print(indentation_level + 1);
    cout << setw(indentation_level) << " " << "} " << endl;
  }

private:
  string m_op;
  unique_ptr<Expression> m_operand;
};

class AssigmentOperator : public Expression {
public:
  AssigmentOperator(const Token& identifier, const Token& op, unique_ptr<Expression> value):
    m_identifier(identifier.lexemes), m_op(op.lexemes), m_value(std::move(value)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "AssigmentOperator { " << '\n';
    cout << setw(indentation_level + 2) << " " << "identifier: " << m_identifier << "\n";
    cout << setw(indentation_level + 2) << " " << "operator: " << m_op << "\n";
    cout << setw(indentation_level + 2) << " " << "value: ";
    m_value->print(indentation_level + 1);
    cout << setw(indentation_level) << " " << "} " << endl;
  }

private:
  string m_identifier;
  string m_op;
  unique_ptr<Expression> m_value;
};

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

class Variable : public ASTNode {
public:

  Variable(const Token& keyword, const Token& type, const Token& identifier, const Token& assignment, unique_ptr<Expression> value, const Token& semicolon):
    m_keyword(keyword.lexemes), m_type(type.lexemes), m_identifier(identifier.lexemes), m_assignment(assignment.lexemes), m_value(std::move(value)), m_semicolon(semicolon.lexemes) {}
  Variable(const Token& keyword, const Token& type, const Token& identifier, const Token& semicolon):
    m_keyword(keyword.lexemes), m_type(type.lexemes), m_identifier(identifier.lexemes), m_semicolon(semicolon.lexemes) {}

  void print(int indentation_level = 0) const override {
    if (m_assignment.empty()) {
      cout << '\n' << setw(indentation_level) << " " << "Variable-Declaration { " << '\n';
      cout << setw(indentation_level + 2) << " " << "kind: " << m_keyword << '\n';
      cout << setw(indentation_level + 2) << " " << "identifier: " << m_identifier << '\n';
      cout << setw(indentation_level + 2) << " " << "type: " << m_type << '\n';
      cout << setw(indentation_level + 2) << " " << "value: null" << '\n';
      cout << setw(indentation_level) << " " << "} " << endl;
    }
    else {
      cout << '\n' << setw(indentation_level) << " " << "Variable-Initialization { " << '\n';
      cout << setw(indentation_level + 2) << " " << "kind: " << m_keyword << '\n';
      cout << setw(indentation_level + 2) << " " << "identifier: " << m_identifier << '\n';
      cout << setw(indentation_level + 2) << " " << "type: " << m_type << '\n';
      cout << setw(indentation_level + 2) << " " << "value: ";
      m_value->print(indentation_level + 2);
      cout << setw(indentation_level) << " " << "} " << endl;
    }
  }

private:
  string m_keyword;
  string m_type;
  string m_identifier;
  string m_assignment;
  unique_ptr<Expression> m_value;
  string m_semicolon;

};

class BlockStatement : public ASTNode {
public:
  BlockStatement(vector<unique_ptr<ASTNode>> statements):
    m_statements(std::move(statements)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "BlockStatement { " << '\n';
    cout << setw(indentation_level + 2) << " " << "content: ";
    for(const auto& statement: m_statements){
      cout << setw(indentation_level + 2) << " ";
      statement->print(indentation_level + 4);
    }
    cout << setw(indentation_level) << " " << "} " << endl;
  }

private:
  vector<unique_ptr<ASTNode>> m_statements;
};

class ControlFlow : public ASTNode {};

class IfStatement: public ControlFlow {
public:
  IfStatement(unique_ptr<Expression> condition, unique_ptr<BlockStatement> block):
    m_condition(std::move(condition)), m_block(std::move(block)){}

  void print(int indentation_level) const override {
    cout << '\n' << setw(indentation_level) << " " << "If Statement { " << '\n';
    cout << setw(indentation_level + 2) << " " << "condition: ";
    m_condition->print(indentation_level + 2);
    m_block->print(indentation_level + 2);
  }

private:
  unique_ptr<Expression> m_condition;
  unique_ptr<BlockStatement> m_block;
};

class ElseStatement: public ControlFlow {

};
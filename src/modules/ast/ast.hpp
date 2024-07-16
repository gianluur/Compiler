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

class Integer : public Literal {
public:
  Integer(const Token& token) : m_token(token.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "LiteralInteger { " << '\n';
    cout << setw(indentation_level + 2) << " " << "value: " << m_token << "\n";
    cout <<  setw(indentation_level) << " " << "} " << endl;
  }

private:
  string m_token;
};

class Float : public Literal {
public:
  Float(const Token& token) : m_token(token.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "LiteralFloat { " << '\n';
    cout << setw(indentation_level + 2) << " " << "value: " << m_token << "\n";
    cout <<  setw(indentation_level) << " " << "} " << endl;
  }

private:
  string m_token;
};

class String: public Literal {
public:
  String(const Token& token) : m_token(token.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "LiteralString { " << '\n';
    cout << setw(indentation_level + 2) << " " << "value: " << m_token << "\n";
    cout <<  setw(indentation_level) << " " << "} " << endl;
  }
private:
  string m_token;
};

class Char : public Literal {
public:
  Char(const Token& token) : m_token(token.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "LiteralChar { " << '\n';
    cout << setw(indentation_level + 2) << " " << "value: " << m_token << "\n";
    cout <<  setw(indentation_level) << " " << "} " << endl;
  }
private:
  string m_token;
};

class Boolean : public Literal {
public:
  Boolean(const Token& token) : m_token(token.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "LiteralBoolean { " << '\n';
    cout << setw(indentation_level + 2) << " " << "value: " << m_token << "\n";
    cout <<  setw(indentation_level) << " " << "} " << endl;
  }
private:
  string m_token;
};

class Identifier : public Expression {
public:
  Identifier(const Token& token) : m_token(token.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Identifier { " << '\n';
    cout << setw(indentation_level + 2) << " " << "name: " << m_token << "\n";
    cout <<  setw(indentation_level) << " " << "} " << endl;
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
    m_value->print(indentation_level + 4);
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
    m_left->print(indentation_level + 4);
    cout << setw(indentation_level + 2) << " " << "right: ";
    m_right->print(indentation_level + 4);
    cout <<  setw(indentation_level) << " " << "} " << endl;
  }

private:
  unique_ptr<Expression> m_left;
  string m_op;
  unique_ptr<Expression> m_right;

};

class Variable : public ASTNode {
public:

  Variable(const Token& keyword, const Token& type, const Token& identifier, unique_ptr<Expression> value):
    m_keyword(keyword.lexemes), m_type(type.lexemes), m_identifier(identifier.lexemes), m_value(std::move(value)) {}
  Variable(const Token& keyword, const Token& type, const Token& identifier):
    m_keyword(keyword.lexemes), m_type(type.lexemes), m_identifier(identifier.lexemes) {}

  void print(int indentation_level = 0) const override {
    if (m_value) {
      cout << '\n' << setw(indentation_level) << " " << "Variable-Initialization { " << '\n';
      cout << setw(indentation_level + 2) << " " << "kind: " << m_keyword << '\n';
      cout << setw(indentation_level + 2) << " " << "identifier: " << m_identifier << '\n';
      cout << setw(indentation_level + 2) << " " << "type: " << m_type << '\n';
      cout << setw(indentation_level + 2) << " " << "value: ";
      m_value->print(indentation_level + 4);
      cout << setw(indentation_level) << " " << "} " << endl;
    }
    else {
      cout << '\n' << setw(indentation_level) << " " << "Variable-Declaration { " << '\n';
      cout << setw(indentation_level + 2) << " " << "kind: " << m_keyword << '\n';
      cout << setw(indentation_level + 2) << " " << "identifier: " << m_identifier << '\n';
      cout << setw(indentation_level + 2) << " " << "type: " << m_type << '\n';
      cout << setw(indentation_level + 2) << " " << "value: null" << '\n';
      cout << setw(indentation_level) << " " << "} " << endl;
    }
  }

private:
  string m_keyword;
  string m_type;
  string m_identifier;
  string m_assignment;
  unique_ptr<Expression> m_value;

};

class BlockStatement : public ASTNode {
public:
  BlockStatement(vector<unique_ptr<ASTNode>> statements):
    m_statements(std::move(statements)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "BlockStatement { " << '\n';
    cout << setw(indentation_level + 2) << " " << "content: ";
    for(const auto& statement: m_statements){
      statement->print(indentation_level + 4);
    }
    cout << '\n' << setw(indentation_level) << " " << "} " << endl;
  }

private:
  vector<unique_ptr<ASTNode>> m_statements;
};

class ControlFlow : public ASTNode {};

class ElseStatement: public ControlFlow {
public:
  ElseStatement(unique_ptr<BlockStatement> body):
    m_body(std::move(body)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Else Statement { ";
    m_body->print(indentation_level + 2);        
    cout << setw(indentation_level) << " " << "} " << endl;
  }

private:
  unique_ptr<BlockStatement> m_body;
};

class IfStatement: public ControlFlow {
public:
  IfStatement(unique_ptr<Expression> condition, unique_ptr<BlockStatement> body):
    m_condition(std::move(condition)), m_body(std::move(body)) {}

  IfStatement(unique_ptr<Expression> condition, unique_ptr<BlockStatement> body, unique_ptr<ElseStatement> elseBody):
    m_condition(std::move(condition)), m_body(std::move(body)), m_else(std::move(elseBody)) {}

  void print(int indentation_level) const override {
    cout << '\n' << setw(indentation_level) << " " << "If Statement { " << '\n';
    cout << setw(indentation_level + 2) << " " << "condition: ";
    m_condition->print(indentation_level + 4);
    cout << setw(indentation_level + 2) << " " << "body: ";
    m_body->print(indentation_level + 4);

    if (m_else) {
      cout << setw(indentation_level + 2) << " " << "else: ";
      m_else->print(indentation_level + 4);
    }
    cout << setw(indentation_level) << " " << "} ";
  }

private:
  unique_ptr<Expression> m_condition;
  unique_ptr<BlockStatement> m_body;
  unique_ptr<ElseStatement> m_else;
};

class Loops: public ASTNode {};

class While: public Loops {
public:
  While(unique_ptr<Expression> condition, unique_ptr<BlockStatement> body):
    m_condition(std::move(condition)), m_body(std::move(body)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "While Statement {" << '\n';
    cout << setw(indentation_level + 2) << " " << "condition: ";
    m_condition->print(indentation_level + 4);
    cout << setw(indentation_level + 2) << " " << "body: ";
    m_body->print(indentation_level + 4);
    cout << setw(indentation_level) << " " << "} " << endl;
  }

private:
  unique_ptr<Expression> m_condition;
  unique_ptr<BlockStatement> m_body;

};

class Do: public Loops {
public:
  Do(unique_ptr<BlockStatement> body, unique_ptr<Expression> condition):
    m_body(std::move(body)), m_condition(std::move(condition)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Do {\n";
    cout << setw(indentation_level + 2) << " " << "body: ";
    m_body->print(indentation_level + 4);
    cout << setw(indentation_level + 2) << " " << "condition: ";
    m_condition->print(indentation_level + 4);
    cout << setw(indentation_level) << " " << "}" << '\n';
  }

private:
  unique_ptr<BlockStatement> m_body;
  unique_ptr<Expression> m_condition;
};

class For: public Loops {
public:
  For(unique_ptr<Variable> initialization, unique_ptr<Expression> condition, unique_ptr<AssigmentOperator> update, unique_ptr<BlockStatement> body):
    m_initialization(std::move(initialization)), m_condition(std::move(condition)), m_update(std::move(update)), m_body(std::move(body)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "For Statement {" << '\n';
    cout << setw(indentation_level + 2) << " " << "initialization: ";
    m_initialization->print(indentation_level + 4);
    cout << setw(indentation_level + 2) << " " << "condition: ";
    m_condition->print(indentation_level + 4);
    cout << setw(indentation_level + 2) << " " << "update: ";
    m_update->print(indentation_level + 4);
    m_body->print(indentation_level + 2);
    cout << setw(indentation_level) << " " << "} " << endl;
  }

private:
  unique_ptr<Variable> m_initialization;
  unique_ptr<Expression> m_condition;
  unique_ptr<AssigmentOperator> m_update;
  unique_ptr<BlockStatement> m_body;

};

class Parameter: public ASTNode {
public:
  Parameter(const Token& type, unique_ptr<Identifier> name):
    m_type(type.lexemes), m_name(std::move(name)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Parameter {" << '\n';
    cout << setw(indentation_level + 2) << " " << "type: " << m_type << '\n';
    cout << setw(indentation_level + 2) << " " << "name: ";
    m_name->print(indentation_level + 4);
    cout << setw(indentation_level) << " " << "} " << endl;
  }

private:
  string m_type;
  unique_ptr<Identifier> m_name;
};

class Function: public ASTNode {
public:
  Function(const Token& returnType, unique_ptr<Identifier> name, vector<unique_ptr<Parameter>> parameters, unique_ptr<BlockStatement> body):
    m_returnType(returnType.lexemes), m_name(std::move(name)), m_parameters(std::move(parameters)), m_body(std::move(body)) {}

  void print(int indentation_level) const override {
    cout << '\n' << setw(indentation_level) << " " << "Function {" << '\n';
    cout << setw(indentation_level + 2) << " " << "returnType: " << m_returnType << '\n';
    cout << setw(indentation_level + 2) << " " << "name: ";
    m_name->print(indentation_level + 4);
    for(const auto& parameter : m_parameters) {
      parameter->print(indentation_level + 4);
    }
    cout << setw(indentation_level + 2) << " " << "body: ";
    m_body->print(indentation_level + 4);
    cout << setw(indentation_level) << " " << "} " << endl;
  }

private:

  string m_returnType;
  unique_ptr<Identifier> m_name;
  vector<unique_ptr<Parameter>> m_parameters;
  unique_ptr<BlockStatement> m_body;

};

class Struct : public ASTNode {
public:
  Struct(unique_ptr<Identifier> name, unique_ptr<BlockStatement> body):
    m_name(std::move(name)), m_body(std::move(body)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Struct {" << '\n';
    cout << setw(indentation_level + 2) << " " << "name: ";
    m_name->print(indentation_level + 4);
    cout << setw(indentation_level + 2) << " " << "body: ";
    m_body->print(indentation_level + 4);
    cout << setw(indentation_level) << " " << "} " << endl;
  }

private:
  unique_ptr<Identifier> m_name;
  unique_ptr<BlockStatement> m_body;
};
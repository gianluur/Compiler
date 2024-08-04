#pragma once

#include <iostream>
#include <vector>
#include <iomanip>
#include <memory>

#include "./token.hpp"

using std::cout, std::endl, std::setw;
using std::unique_ptr, std::make_unique;

class ASTNode {
public:
  virtual ~ASTNode() = default;
  virtual void print(int indentation_level = 0) const = 0;
};

class Expression : public ASTNode {};
class Literal : public Expression {};

class Null: public Literal {
public:
  Null() {}
  
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Null { " << '\n';
    cout << setw(indentation_level + 2) << " " << "value: null" << "\n";
    cout <<  setw(indentation_level) << " " << "} " << endl;
  }

  string getType() const {
    return "null";
  }
};

class Integer : public Literal {
public:
  Integer(const Token& value) : m_value(value.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Integer { " << '\n';
    cout << setw(indentation_level + 2) << " " << "value: " << m_value << "\n";
    cout <<  setw(indentation_level) << " " << "} " << endl;
  }

  string getType() const {
    return "int";
  }

  string getValue() const {
    return m_value;
  }

private:
  string m_value;
};

class Float : public Literal {
public:
  Float(const Token& value) : m_value(value.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "LiteralFloat { " << '\n';
    cout << setw(indentation_level + 2) << " " << "value: " << m_value << "\n";
    cout <<  setw(indentation_level) << " " << "} " << endl;
  }

  string getType() const{
    return "float";
  }

  string getValue() const {
    return m_value;
  }

private:
  string m_value;
};

class String: public Literal {
public:
  String(const Token& value) : m_value(value.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "LiteralString { " << '\n';
    cout << setw(indentation_level + 2) << " " << "value: " << m_value << "\n";
    cout <<  setw(indentation_level) << " " << "} " << endl;
  }

  string getType() const {
    return "string";
  }

  string getValue() const {
    return m_value;
  }

private:
  string m_value;
};

class Char : public Literal {
public:
  Char(const Token& value) : m_value(value.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "LiteralChar { " << '\n';
    cout << setw(indentation_level + 2) << " " << "value: " << m_value << "\n";
    cout <<  setw(indentation_level) << " " << "} " << endl;
  }

  string getType() const {
    return "char";
  }

  string getValue() const {
    return m_value;
  }

private:
  string m_value; 
};

class Boolean : public Literal {
public:
  Boolean(const Token& value) : m_value(value.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "LiteralBoolean { " << '\n';
    cout << setw(indentation_level + 2) << " " << "value: " << m_value << "\n";
    cout <<  setw(indentation_level) << " " << "} " << endl;
  }

  string getType() const {
    return "bool";
  }

  string getValue() const {
    return m_value;
  }

private:
  string m_value;
};

class Cast : public Expression {
public:
  Cast(unique_ptr<Expression> expr, const string& targetType) 
    : m_expr(std::move(expr)), m_targetType(targetType) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Cast {" << '\n';
    cout << setw(indentation_level + 2) << " " << "targetType: " << m_targetType << '\n';
    cout << setw(indentation_level + 2) << " " << "expression: ";
    m_expr->print(indentation_level + 4);
    cout << setw(indentation_level) << " " << "}" << endl;
  }

  string getTargetType() const {
    return m_targetType;
  }

  Expression* getExpression() const {
    return m_expr.get();
  }

private:
  unique_ptr<Expression> m_expr;
  string m_targetType;
};

class Identifier : public Expression {
public:
  Identifier(const Token& name) : m_name(name.lexemes) {}
  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Identifier { " << '\n';
    cout << setw(indentation_level + 2) << " " << "name: " << m_name << "\n";
    cout <<  setw(indentation_level) << " " << "} " << endl;
  }

  string getName() const {
    return m_name;
  }

private:
  string m_name;
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

  string getOperator() const {
    return m_op;
  }

  Expression* getOperand() const {
    return m_operand.get();
  }

  void setOperand(unique_ptr<Expression> operand)  {
    m_operand = std::move(operand);
  }

  Expression* releaseOperand(){
    return m_operand.release();
  }

private:
  string m_op;
  unique_ptr<Expression> m_operand;
};

class AssigmentOperator : public Expression {
public:
  AssigmentOperator(unique_ptr<Identifier> identifier, const Token& op, unique_ptr<Expression> value):
    m_identifier(std::move(identifier)), m_op(op.lexemes), m_value(std::move(value)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "AssigmentOperator { " << '\n';
    cout << setw(indentation_level + 2) << " " << "identifier: ";
    m_identifier->print(indentation_level + 4);
    cout << setw(indentation_level + 2) << " " << "operator: " << m_op << "\n";
    cout << setw(indentation_level + 2) << " " << "value: ";
    m_value->print(indentation_level + 4);
    cout << setw(indentation_level) << " " << "} " << endl;
  }

  Identifier* getIdentifier() const {
    return m_identifier.get();
  }

  Expression* getValue() const {
    return m_value.get();
  }
  
  Expression* relaseValue() {
    return m_value.release();
  }

  void setValue(unique_ptr<Expression> value){
    m_value = std::move(value);
  }

  string getOperator() const {
    return m_op;
  }

private:
  unique_ptr<Identifier> m_identifier;
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

  string getOperator() const {
    return m_op;
  }

  Expression* getLeftOperand() const {
    return m_left.get();
  }

  Expression* getRightOperand() const {
    return m_right.get();
  }

  void setLeftOperand(unique_ptr<Expression> left)  {
    m_left = std::move(left);
  }

  void setRightOperand(unique_ptr<Expression> right)  {
    m_right = std::move(right);
  }

  Expression* releaseLeftOperand() {
      return m_left.release();
    }

    Expression* releaseRightOperand() {
      return m_right.release();
    }

private:
  unique_ptr<Expression> m_left;
  string m_op;
  unique_ptr<Expression> m_right;

};

class Variable : public ASTNode {
public:

  Variable(const Token& keyword, const Token& type, unique_ptr<Identifier> name, unique_ptr<Expression> value):
    m_keyword(keyword.lexemes), m_type(type.lexemes), m_name(std::move(name)), m_value(std::move(value)) {}
  Variable(const Token& keyword, const Token& type, unique_ptr<Identifier> name):
    m_keyword(keyword.lexemes), m_type(type.lexemes), m_name(std::move(name)), m_value(make_unique<Null>()) {}

  void print(int indentation_level = 0) const override {
    if (m_value) {
      cout << '\n' << setw(indentation_level) << " " << "Variable-Initialization { " << '\n';
      cout << setw(indentation_level + 2) << " " << "kind: " << m_keyword << '\n';
      cout << setw(indentation_level + 2) << " " << "name: ";
      m_name->print(indentation_level + 4);
      cout << setw(indentation_level + 2) << " " << "type: " << m_type << '\n';
      cout << setw(indentation_level + 2) << " " << "value: ";
      m_value->print(indentation_level + 4);
      cout << setw(indentation_level) << " " << "} " << endl;
    }
    else {
      cout << '\n' << setw(indentation_level) << " " << "Variable-Declaration { " << '\n';
      cout << setw(indentation_level + 2) << " " << "kind: " << m_keyword << '\n';
      cout << setw(indentation_level + 2) << " " << "identifier: ";
      m_name->print(indentation_level + 4);
      cout << setw(indentation_level + 2) << " " << "type: " << m_type << '\n';
      cout << setw(indentation_level + 2) << " " << "value: null" << '\n';
      cout << setw(indentation_level) << " " << "} " << endl;
    }
  }

  string getKeyword() const {
    return m_keyword;
  }
  
  string getType() const {
    return m_type;
  }

  string getName() const {
    return m_name.get()->getName();
  }

  void setValue(unique_ptr<Expression> value){
    m_value = std::move(value);
  }

  Expression* getValue() const {
    return m_value.get();
  }

  Expression* relaseValue() {
    return m_value.release();
  }

private:
  string m_keyword;
  string m_type;
  unique_ptr<Identifier> m_name;
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

  vector<ASTNode*> getStatements() const {
    vector<ASTNode*> statements;
    for(const auto& statement: m_statements){
      statements.emplace_back(statement.get());
    }
    return statements;
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

  BlockStatement* getBody() const {
    return m_body.get();
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

  Expression* releaseCondition() {
    return m_condition.release();
  }

  Expression* getCondition() const {
    return m_condition.get();
  }

  BlockStatement* getBody() const {
    return m_body.get();
  }

  ElseStatement* getElseStatement() const {
    return m_else.get();
  }

  void setCondition(unique_ptr<Expression> condition){
    m_condition = std::move(condition);
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

  Expression* releaseCondition() {
    return m_condition.release();
  }

  Expression* getCondition() const {
    return m_condition.get();
  }

  BlockStatement* getBody() const {
    return m_body.get();
  }

  void setCondition(unique_ptr<Expression> condition){
    m_condition = std::move(condition);
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

  Expression* releaseCondition() {
    return m_condition.release();
  }

  Expression* getCondition() const {
    return m_condition.get();
  }

  BlockStatement* getBody() const {
    return m_body.get();
  }

  void setCondition(unique_ptr<Expression> condition){
    m_condition = std::move(condition);
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

  Variable* getInitialization() const {
    return m_initialization.get();
  }

  Expression* getCondition() const {
    return m_condition.get();
  }

  AssigmentOperator* getUpdate() const {
    return m_update.get();
  }

  BlockStatement* getBody() const {
    return m_body.get();
  }

  Expression* releaseCondition() {
    return m_condition.release();
  }

  void setCondition(unique_ptr<Expression> condition){
    m_condition = std::move(condition);
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

  string getType() const {
    return m_type;
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

  string getType() const {
    return m_returnType;
  }

  Identifier* getIdentifier() const {
    return m_name.get();
  }

  vector<Parameter*> getParameters() const {
    vector<Parameter*> parameters;
    for (const auto& parameter: m_parameters){
      parameters.emplace_back(parameter.get());
    }
    return parameters;
  }

  BlockStatement* getBody() const {
    return m_body.get();
  }

private:

  string m_returnType;
  unique_ptr<Identifier> m_name;
  vector<unique_ptr<Parameter>> m_parameters;
  unique_ptr<BlockStatement> m_body;

};

class FunctionCall: public ASTNode {
public:
  FunctionCall(unique_ptr<Identifier> name,  vector<unique_ptr<Expression>> arguments):
    m_name(std::move(name)), m_arguments(std::move(arguments)) {} 

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Function Call{" << '\n';
    cout << setw(indentation_level + 2) << " " << "name: ";
    m_name->print(indentation_level + 4);
    cout << setw(indentation_level + 2) << " " << "arguments: ";

    if (m_arguments.empty()){
      cout << '\n' << setw(indentation_level) << " " << "}\n";
    }
    else {
      for(const auto& argument : m_arguments){
        argument->print(indentation_level + 4);
        cout << setw(indentation_level) << " " << "}\n";
      }
    }
  }

  Identifier* getIdentifier() const {
    return m_name.get();
  }

  vector<Expression*> getArguments() const {
    vector<Expression*> arguments;
    for(const auto& argument : m_arguments){
      arguments.emplace_back(argument.get());
    }
    return arguments;
  }

private:
  unique_ptr<Identifier> m_name;
  vector<unique_ptr<Expression>> m_arguments;
};

class Return: public ASTNode {
public:
  Return(unique_ptr<Expression> value):
    m_value(std::move(value)) {}

  void print(int indentation_level = 0) const override {
    cout << '\n' << setw(indentation_level) << " " << "Return {\n";
    cout << setw(indentation_level + 2) << " " << "value: ";
    m_value->print(indentation_level + 4);
    cout << setw(indentation_level) << "}\n";
  }

  Expression* getValue() const {
    return m_value.get();
  }

  Expression* relaseValue(){
    return m_value.release();
  }

  void setValue(unique_ptr<Expression> value) {
    m_value = std::move(value);
  }

private:
  unique_ptr<Expression> m_value;
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

  BlockStatement* getBody() const {
    return m_body.get();
  }

private:
  unique_ptr<Identifier> m_name;
  unique_ptr<BlockStatement> m_body;
};
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "ast.hpp"
#include "scope.hpp"
#include "error.hpp"

using std::cout;
using std::vector, std::string, std::unordered_map; 
using std::unique_ptr, std::make_unique;

class Semantics {
public:
  Semantics(size_t& line): m_scopes(make_unique<Scope>()), m_line(line) {}

  void analyzeVariable(Variable* variable){
    string keyword = variable->getKeyword();
    string type = variable->getType();
    string name = variable->getName();
    Expression* value = variable->getValue();

    m_scopes->declare(name, Symbol(keyword, type));

    if (!dynamic_cast<Null*>(value)){
      const string valueType = checkExpressionType(type, value);
      castValueIfNeeeded(variable, type, valueType);
    }        
  }

  void analyzeAssignmentOperator(AssigmentOperator* statement){
    Identifier* identifier = statement->getIdentifier();
    Expression* value = statement->getValue();

    if (!m_scopes->isDeclared(identifier->getName()))
      error("Identifier: " + identifier->getName() + " is not declared", m_line);
    
    if (m_scopes->find(identifier->getName()).keyword == "const")
      error("Can't reassign to a constant", m_line);

    const string type = m_scopes->find(identifier->getName()).type;
    const string valueType = checkExpressionType(type, value);
    castValueIfNeeeded(statement, type, valueType);
    
  }

  void analyzeFunction(Function* function){
    Identifier* identifier = function->getIdentifier();
    string type = function->getType();
    vector<Parameter*> parameters = function->getParameters();
    BlockStatement* body = function->getBody();

    m_scopes->declare(identifier->getName(), Symbol("func", type, parameters));
    checkReturnType(body, type);

    m_scopes->enterScope();
    analyzeBody(function);
    m_scopes->exitScope();
  }

  void analyzeFunctionCall(FunctionCall* call){
    Identifier* identifier = call->getIdentifier();
    const string name = identifier->getName();

    if (!m_scopes->isDeclared(name))
      error("Identifier: " + name + " is not declared", m_line);

    vector<Expression*> arguments = call->getArguments();
    const int n_arguments = arguments.size();

    vector<Parameter*> parameters = m_scopes->find(name).params;
    const int n_parameters = parameters.size();
    
    if (n_arguments != n_parameters)
      error("Error: Function " + name + " was expecting " + std::to_string(n_parameters) + " but instead got only " + std::to_string(n_arguments), m_line);
    
    for(int i = 0; i < n_parameters; i++){
      string expectedType = parameters[i]->getType();
      string valueType = checkExpressionType(expectedType, arguments[i]); 
      if (expectedType != valueType) 
        error("Type Mismatch: Expected " + expectedType + " but got " + valueType, m_line);
    }
  }

  void analyzeIfStatement(IfStatement* statement){
    analyzeConditionalStatement(statement);
    ElseStatement* elseStatement = statement->getElseStatement();
    if (elseStatement != nullptr)
      analyzeBody(elseStatement);
  }

  template <typename T>
  void analyzeConditionalStatement(T* statement){
    analyzeCondition(statement);

    m_scopes->enterScope();
    analyzeBody(statement);
    m_scopes->exitScope();
  }

  void analyzeFor(For* statement){  
    analyzeVariable(statement->getInitialization());
    analyzeCondition(statement);      
    analyzeAssignmentOperator(statement->getUpdate());

    m_scopes->enterScope();
    analyzeBody(statement);
    m_scopes->exitScope();
  }

private:
   unique_ptr<Scope> m_scopes;
   size_t& m_line;

  void analyzeNode(ASTNode* current) {
    if (current == nullptr) return; 

    if (Variable* variable = dynamic_cast<Variable*>(current)){
      analyzeVariable(variable);
    }
    else if (AssigmentOperator* assignmentOperator = dynamic_cast<AssigmentOperator*>(current)){
      analyzeAssignmentOperator(assignmentOperator);
    }

    else if (Function* function = dynamic_cast<Function*>(current)){
      analyzeFunction(function);
    }
    
    else if (FunctionCall* call = dynamic_cast<FunctionCall*>(current)){
      analyzeFunctionCall(call);
    }

    else if (IfStatement* statement = dynamic_cast<IfStatement*>(current)){
      analyzeConditionalStatement(statement);
    }

    else if (While* statement = dynamic_cast<While*>(current)){
      analyzeConditionalStatement(statement);
    }

    else if (Do* statement = dynamic_cast<Do*>(current)){
      analyzeConditionalStatement(statement);
    }

    else if (For* statement = dynamic_cast<For*>(current)){
      analyzeFor(statement);
    }

    // else if (Struct* statement = dynamic_cast<Struct*>(current)){
    //   cout << "struct "; //TODO
    // }
  }

  void checkReturnType(BlockStatement* body, string type){
    vector<ASTNode*> statements = body->getStatements();
    for(const auto& current: statements) {
      if (Return* statement = dynamic_cast<Return*>(current)) {
        if (type == "null")
          error("Return statement found in a function returning null", m_line);

        const string valueType = checkExpressionType(type, statement->getValue());
        castValueIfNeeeded(statement, type, valueType);
      }
    }
  }

  template <typename T>
  void analyzeBody(T* statement) {
    BlockStatement* body = statement->getBody();
    for(ASTNode* current : body->getStatements()){
      analyzeNode(current);
    }
  }

  template <typename T>
  void analyzeCondition(T* statement) {
    auto* condition = statement->getCondition();
    const string expectedType = "bool";
    const string valueType = checkExpressionType(expectedType, condition);

    if (valueType != expectedType){
      if (valueType == "int" || valueType == "float") {
          unique_ptr<Expression> condition(statement->releaseCondition());
          auto valueToCast = make_unique<Cast>(std::move(condition), expectedType);
          statement->setCondition(std::move(valueToCast));
      } 
      else {
          error("Condition must always evaluate to boolean", m_line);
      }
    }
  }

  template <typename T>
  void castValueIfNeeeded(T* statement, const string& type, const string& valueType){
    if ((type == "int" && valueType == "float") || (type == "int" && valueType == "bool") || (type == "int" && valueType == "char")){
      unique_ptr<Expression> valueToCast(statement->relaseValue());
      auto toCast = make_unique<Cast>(std::move(valueToCast), "int");
      statement->setValue(std::move(toCast));
    }
    else if (type == "float" && (valueType == "int" || valueType == "bool")){
      unique_ptr<Expression> valueToCast(statement->relaseValue());
      auto toCast = make_unique<Cast>(std::move(valueToCast), "float");
      statement->setValue(std::move(toCast));
    }
    else if (type == "bool" && (valueType == "int" || valueType == "float")){
      unique_ptr<Expression> valueToCast(statement->relaseValue());
      auto toCast = make_unique<Cast>(std::move(valueToCast), "bool");
      statement->setValue(std::move(toCast));
    }
    else if (type == "char" && (valueType == "int" || valueType == "bool")){
      unique_ptr<Expression> valueToCast(statement->relaseValue());
      auto toCast = make_unique<Cast>(std::move(valueToCast), "char");
      statement->setValue(std::move(toCast));
    }
    else if (type != valueType) 
      error("Type Mismatch: Expected " + type + " but got " + valueType, m_line);
  }

  void castBinaryOperatorOperands(BinaryOperator* binaryOperator, string& leftType, string& rightType, const string& op){
    if ((string("+-*/%").find(op)) != string::npos) {
      if ((leftType == "int" && rightType == "float")){
        unique_ptr<Expression> operand(binaryOperator->releaseLeftOperand());
        auto toCast = make_unique<Cast>(std::move(operand), "float");
        binaryOperator->setLeftOperand(std::move(toCast));
        leftType = "float";
      }
      else if (leftType == "float" && rightType == "int") {
        unique_ptr<Expression> operand(binaryOperator->releaseRightOperand());
        auto toCast = make_unique<Cast>(std::move(operand), "float");
        binaryOperator->setRightOperand(std::move(toCast));
        rightType = "float";
      }
      else if ((leftType == "char" || leftType == "bool") && rightType == "int"){
        unique_ptr<Expression> operand(binaryOperator->releaseLeftOperand());
        auto toCast = make_unique<Cast>(std::move(operand), "int");
        binaryOperator->setLeftOperand(std::move(toCast));
        leftType = "int";
      }
      else if (leftType == "int" && (rightType == "char" || rightType == "bool")){
        unique_ptr<Expression> operand(binaryOperator->releaseRightOperand());
        auto toCast = make_unique<Cast>(std::move(operand), "int");
        binaryOperator->setRightOperand(std::move(toCast));
        rightType = "int";
      }
      else if ((leftType == "char" && (rightType == "float" || rightType == "bool")) || (rightType == "char" && (leftType == "float" || leftType == "bool"))){
        unique_ptr<Expression> left(binaryOperator->releaseLeftOperand());
        auto toCastLeft = make_unique<Cast>(std::move(left), "int");
        binaryOperator->setLeftOperand(std::move(toCastLeft));
        leftType = "int";

        unique_ptr<Expression> right(binaryOperator->releaseRightOperand());
        auto toCastRight = make_unique<Cast>(std::move(right), "int");
        binaryOperator->setRightOperand(std::move(toCastRight));
        rightType = "int";
      }
      else  
        error("Math operations is allowed only for int/floats or type castable into them", m_line); 
    }
    else
    {
      if (leftType == "int"){
        unique_ptr<Expression> operand(binaryOperator->releaseLeftOperand());
        auto toCast = make_unique<Cast>(std::move(operand), "bool");
        binaryOperator->setLeftOperand(std::move(toCast));
        leftType = "bool";
      }

      if (rightType == "int"){
        unique_ptr<Expression> operand(binaryOperator->releaseRightOperand());
        auto toCast = make_unique<Cast>(std::move(operand), "bool");
        binaryOperator->setRightOperand(std::move(toCast));
        rightType = "bool";
      }
    }
  }

  string checkExpressionType(const string& expectedType, Expression* value) {
    const string valueType = analyzeOperand(value);
    return valueType;
  }

  string getIdentifierType(Identifier* identifier){
    const string name = identifier->getName();
    if (!m_scopes->isDeclared(name))
      error("Identifier: " + name + " is not declared", m_line);
    return m_scopes->find(name).type;
  }

  string analyzeUnaryOperator(UnaryOperator* unaryOperator){
    string type = analyzeOperand(unaryOperator->getOperand());
    if (type == "int" || type == "float" || type == "char"){
      unique_ptr<Expression> operand(unaryOperator->releaseOperand());
      auto toCast = make_unique<Cast>(std::move(operand), "bool");
      unaryOperator->setOperand(std::move(toCast));
      type = "bool";
    }
    return type;

  }

  string analyzeBinaryOperator(BinaryOperator* binaryOperator) {
    string leftType = analyzeOperand(binaryOperator->getLeftOperand());
    string rightType = analyzeOperand(binaryOperator->getRightOperand());
    string op = binaryOperator->getOperator();

    castBinaryOperatorOperands(binaryOperator, leftType, rightType, op);

    if (leftType != rightType)
      error("Type Mismatch in binary operator: " + leftType + " and " + rightType + " do not match", m_line);

    return leftType; // Assuming binary operator returns the type of its operands
  }

  string analyzeOperand(Expression* operand){
    if (dynamic_cast<Integer*>(operand)) return "int";
    else if (dynamic_cast<Float*>(operand)) return "float";
    else if (dynamic_cast<Char*>(operand)) return "char";
    else if (dynamic_cast<String*>(operand)) return "string";
    else if (dynamic_cast<Boolean*>(operand)) return "bool";
    else if (dynamic_cast<Null*>(operand)) return "null";
    else if (Identifier* identifier = dynamic_cast<Identifier*>(operand)) return getIdentifierType(identifier);
    else if (BinaryOperator* binaryOperator = dynamic_cast<BinaryOperator*>(operand)){
      return analyzeBinaryOperator(binaryOperator);
    }
    else if (UnaryOperator* unaryOperator = dynamic_cast<UnaryOperator*>(operand)){
      return analyzeUnaryOperator(unaryOperator); 
    } 
    else { 
      error("Unknown operand type", m_line);
      return "";
    }
  }
};
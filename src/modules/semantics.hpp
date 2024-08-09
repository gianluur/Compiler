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
      const string valueType = getExpressionType(type, value);
      if (valueType != type && handleSmallerTypes(type, valueType))
        error("Expected " + type + " in variable declaration but got " + valueType + " instead", m_line);
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
    const string valueType = getExpressionType(type, value);
    if (valueType != type && handleSmallerTypes(type, valueType))
      error("Expected " + type + " in assignment but got " + valueType + "instead", m_line);
    
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
      string valueType = getExpressionType(expectedType, arguments[i]); 
      if (expectedType != valueType) 
        error("In function call the " + std::to_string(i + 1) + "* parameter was expecting a " + expectedType + "but instead got a " + valueType, m_line);
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

        const string valueType = getExpressionType(type, statement->getValue());

        if (valueType != type && handleSmallerTypes(type, valueType))
          error("Expected " + type + " in return value but got " + valueType + " instead", m_line);
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
    const string valueType = getExpressionType(expectedType, condition);

    if (valueType != expectedType)
      error("Conditions must always evaluate to boolean", m_line);
  }

  bool handleSmallerTypes(const string& expectedType, const string& type){
    if (expectedType == "int" && (type == "int8" || type == "int16" || type == "int32" || type == "int64" || type == "uint8" || type == "uint16" || type == "uint32" || type == "uint64"))
      return true;

    if (expectedType == "float" && (type == "float32" || type == "float64"))
      return true;
    
    return false;
  }

  string getExpressionType(const string& expectedType, Expression* value) {
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
    return type;

  }

  string analyzeBinaryOperator(BinaryOperator* binaryOperator) {
    string leftType = analyzeOperand(binaryOperator->getLeftOperand());
    string rightType = analyzeOperand(binaryOperator->getRightOperand());
    string op = binaryOperator->getOperator();

    if (leftType != rightType)
      error("Type Mismatch in binary operator: " + leftType + " and " + rightType + " do not match", m_line);

    return leftType; // Assuming binary operator returns the type of its operands
  }

  string analyzeOperand(Expression* operand){
    if (dynamic_cast<Integer*>(operand)) 
      return "int";

    else if (dynamic_cast<Float*>(operand)) 
      return "float";

    else if (dynamic_cast<Char*>(operand)) 
      return "char";

    else if (dynamic_cast<String*>(operand)) 
      return "string";

    else if (dynamic_cast<Boolean*>(operand)) 
      return "bool";

    else if (dynamic_cast<Null*>(operand)) 
      return "null";

    else if (Identifier* identifier = dynamic_cast<Identifier*>(operand)) 
      return getIdentifierType(identifier);

    else if (BinaryOperator* binaryOperator = dynamic_cast<BinaryOperator*>(operand))
      return analyzeBinaryOperator(binaryOperator);
    
    else if (UnaryOperator* unaryOperator = dynamic_cast<UnaryOperator*>(operand))
      return analyzeUnaryOperator(unaryOperator); 

    else if (Cast* cast = dynamic_cast<Cast*>(operand))
      return cast->getTargetType();
  
    else { 
      error("Unknown operand type", m_line);
      return "";
    }
  }
};
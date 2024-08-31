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

   unique_ptr<Scope> m_scopes;

  void analyzeVariable(Variable* variable){
    string keyword = variable->getKeyword();
    string type = variable->getType();
    string name = variable->getName();
    Expression* value = variable->getValue();

    m_scopes->declare(name, Symbol(keyword, type));

    if (!dynamic_cast<Null*>(value)){
      const string valueType = getExpressionType(type, value);
      if (valueType != type && !isSmallerType(type, valueType))
        error("Expected " + type + " in variable declaration but got " + valueType + " instead", m_line);
    }        
  }

  void analyzeAssignmentOperator(const string& name, Expression* value){
    if (!m_scopes->isDeclared(name))
      error("Identifier: " + name + " is not declared", m_line);
    
    if (m_scopes->find(name).keyword == "const")
      error("Can't reassign to a constant", m_line);

    const string type = m_scopes->find(name).type;
    const string valueType = getExpressionType(type, value);
    if (valueType != type && !isSmallerType(type, valueType))
      error("Expected " + type + " in assignment but got " + valueType + "instead", m_line);
  }

  void analyzeFunction(Function* function){
    Identifier* identifier = function->getIdentifier();
    const string type = function->getType();
    vector<Parameter*> parameters = function->getParameters();
    m_scopes->declare(identifier->getName(), Symbol("func", type, parameters));
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

  void isNodeScoped(const string keyword, const bool isScoped){
    if (!isScoped){
      const string scope = (keyword == "return") ? "function body" : "loop";
      error("You can't use " + keyword + " outside a " + scope, m_line);
    }
  }

  void analyzeReturn(Expression* value, const string& type){
    if (type == "null")
      error("Return statement found in a function returning null", m_line);
    cout << type << '\n';
    const string valueType = getExpressionType(type, value);
    if (valueType != type && !isSmallerType(type, valueType))
      error("Expected " + type + " in return value but got " + valueType + " instead", m_line);
  }

  void analyzeCondition(Expression* condition) {
    const string expectedType = "bool";
    const string valueType = getExpressionType(expectedType, condition);

    if (valueType != expectedType)
      error("Conditions must always evaluate to boolean", m_line);
  }

private:
   size_t& m_line;

  bool isSmallerType(const string& expectedType, const string& valueType){
    if ((expectedType == "int" || expectedType == "int8" || expectedType == "int16" || 
         expectedType == "int32" || expectedType == "int64" || 
         expectedType == "uint8" || expectedType == "uint16" || 
         expectedType == "uint32" || expectedType == "uint64") &&
        (valueType == "int" || valueType == "int8" || valueType == "int16" ||
         valueType == "int32" || valueType == "int64" || 
         valueType == "uint8" || valueType == "uint16" || 
         valueType == "uint32" || valueType == "uint64")
        ) return true;

    if ((expectedType == "float" || expectedType == "float32" || expectedType == "float64") &&
        (valueType == "float" || valueType == "float32" || valueType == "float64")) {
        return true;
    }
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

  string getFunctionCallType(FunctionCall* functionCall){
    const string name = functionCall->getIdentifier()->getName();
    if (!m_scopes->isDeclared(name))
      error("The function call to " + name + " couldn't be executed because the function definition wasn't found", m_line);
    return m_scopes->find(name).type;
  }

  string analyzeUnaryOperator(UnaryOperator* unaryOperator){
    return analyzeOperand(unaryOperator->getOperand());
  }

  string analyzeBinaryOperator(BinaryOperator* binaryOperator) {
    string leftType = analyzeOperand(binaryOperator->getLeftOperand());
    string rightType = analyzeOperand(binaryOperator->getRightOperand());
    string op = binaryOperator->getOperator();
    
    if (string("+-*/%").find(op) != string::npos){
      if (leftType == "string" && rightType == "string" && op != "+")
        error("String concatenation can be done only with the '+' operator, everything else isn't allowed");
    }
    else {
      leftType = "bool"; rightType = "bool";
    }

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
    
    else if (FunctionCall* functionCall = dynamic_cast<FunctionCall*>(operand)){
      return getFunctionCallType(functionCall);
    }
    else if (Cast* cast = dynamic_cast<Cast*>(operand))
      return cast->getTargetType();
  
    else { 
      error("Unknown operand type", m_line);
      return "";
    }
  }
};
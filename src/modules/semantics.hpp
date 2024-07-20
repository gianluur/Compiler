#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "./ast.hpp"

using std::cout, std::endl;
using std::vector, std::string, std::unordered_map; 
using std::unique_ptr, std::make_unique;

struct Symbol {
  string keyword; // var/const/func
  string type;
  int scopeLevel;

  Symbol() : keyword(""), type(""), scopeLevel(0) {} // Default constructor
  Symbol(string keyword, string type, int ScopeLevel): 
    keyword(keyword), type(type), scopeLevel(scopeLevel) {}
};

class SemanticAnalyzer {
public:
  SemanticAnalyzer(const vector<unique_ptr<ASTNode>>& ast): 
    m_ast(std::move(ast)) {}

  void analyze(){
    cout << "------ Semantics Start -----" << endl << endl;
    for(const unique_ptr<ASTNode>& node : m_ast){
      if (Variable* var = dynamic_cast<Variable*>(node.get())){
        analyzerVariable(var);
      }

      else {
        error("idk error");
      }
    }
    cout << endl << "------ Semantics End -----" << endl;
  }

private:
   const vector<unique_ptr<ASTNode>>& m_ast;
   unordered_map<string, Symbol> symbolTable;

  void error(const string& message){
    cout << message << endl;
    exit(1);
  }

  void analyzerVariable(Variable* var){
    string keyword = var->getKeyword();
    string type = var->getType();
    string name = var->getName();
    Expression* value = var->getValue();

    auto result = symbolTable.emplace(name, Symbol(keyword, type, 0));
    checkVariableRedeclaration(result);
    checkVariableType(keyword, type, name, value);
  }

  void checkVariableRedeclaration(auto result){
    if (!result.second){
      error("Variable redeclaration");
    }
  }

  void checkVariableType(const string& keyword, const string& expectedType, const string& name, Expression* value) {
    if (dynamic_cast<Integer*>(value)){
      if (expectedType != "int")
        error("Type Mismatch: In this variable" + keyword + " " + expectedType + " " + name + "was expected a " + expectedType + " but got a integer instead");
    }

    else if (dynamic_cast<Float*>(value)){
      if (expectedType != "float")
        error("Type Mismatch: In this variable" + keyword + " " + expectedType + " " + name + "was expected a " + expectedType + " but got a float instead");
    }

    else if (dynamic_cast<Char*>(value)){
      if (expectedType != "char")
        error("Type Mismatch: In this variable" + keyword + " " + expectedType + " " + name + "was expected a " + expectedType + " but got a char instead");
    }

    else if (dynamic_cast<String*>(value)){
      if (expectedType != "string")
        error("Type Mismatch: In this variable" + keyword + " " + expectedType + " " + name + "was expected a " + expectedType + " but got a string instead");
    }

    else if (dynamic_cast<Boolean*>(value)){
      if (expectedType != "bool")
        error("Type Mismatch: In this variable" + keyword + " " + expectedType + " " + name + "was expected a " + expectedType + " but got a bool instead");
    }

    else if (Identifier* identifier = dynamic_cast<Identifier*>(value)){
      string value_type = getIdentifierType(identifier);
      if (expectedType != value_type)
        error("Type Mismatch: In this variable" + keyword + " " + expectedType + " " + name + "was expected a " + expectedType + " but got a " + value_type + "instead");
    }

    else if (BinaryOperator* binaryOperator = dynamic_cast<BinaryOperator*>(value)){
      analyzeBinaryOperator(expectedType, binaryOperator);
    }

    else if (UnaryOperator* unaryOperator = dynamic_cast<UnaryOperator*>(value)){
      analyzeUnaryOperator(expectedType, unaryOperator);
    }

    else if (!dynamic_cast<Literal*>(value) && !dynamic_cast<Identifier*>(value)){
      error("Semantic Error: Unsupported expression type");
    }
  }

  string getIdentifierType(Identifier* identifier){
    const string name = identifier->getName();
    if (symbolTable.count(name) > 0){
      return symbolTable[name].type;
    }
    else {
      error("Unknown identifier");
      return "";
    }
  }

  void analyzeUnaryOperator(const string expectedType, UnaryOperator* unaryOperator){
    Expression* operand = unaryOperator->getOperand();
    string operandType = analyzeOperand(operand);

    if (operandType != expectedType){
      error("Type Mismatch in Unary Operator: Expected " + expectedType + " but got " + operandType);
    }
  }

  void analyzeBinaryOperator(const string expectedType, BinaryOperator* binaryOperator) {
    Expression* left = binaryOperator->getLeftOperand();
    Expression* right = binaryOperator->getRightOperand();
    string op = binaryOperator->getOperator();

    string leftType = analyzeOperand(left);
    string rightType = analyzeOperand(right);

    if (leftType != expectedType || rightType != expectedType)
      error("Type Mismatch: In binary operator was expected a " + expectedType + " but got " + leftType + " and " + rightType);
  }

  string analyzeOperand(Expression* operand){
    if (dynamic_cast<Integer*>(operand)) return "int";
    else if (dynamic_cast<Float*>(operand)) return "float";
    else if (dynamic_cast<Char*>(operand)) return "char";
    else if (dynamic_cast<String*>(operand)) return "string";
    else if (dynamic_cast<Boolean*>(operand)) return "bool";
    else if (Identifier* identifier = dynamic_cast<Identifier*>(operand)) return getIdentifierType(identifier);
    else if (BinaryOperator* binaryOperator = dynamic_cast<BinaryOperator*>(operand)){
      string leftType = analyzeOperand(binaryOperator->getLeftOperand());
      string rightType = analyzeOperand(binaryOperator->getRightOperand());
      if (leftType != rightType)
        error("Type Mismatch: in binary operator was expected " + leftType + " and " + rightType + " do not match");
      return leftType; // if this returns it means they have the same type
    }
    else if (UnaryOperator* unaryOperator = dynamic_cast<UnaryOperator*>(operand)){
     return analyzeOperand(unaryOperator->getOperand());
    }
    else error("Unknown Type"); return "";
  }

};
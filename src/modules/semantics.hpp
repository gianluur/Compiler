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
  Symbol(string keyword, string type, int scopeLevel): 
    keyword(keyword), type(type), scopeLevel(scopeLevel) {}
};

class SemanticAnalyzer {
public:
  SemanticAnalyzer(const vector<unique_ptr<ASTNode>>& ast): 
    m_ast(std::move(ast)) {}

  void analyze(){
    cout << "------ Semantics Start -----" << endl << endl;
    for(const unique_ptr<ASTNode>& node : m_ast){
      ASTNode* current = node.get();
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

      else {
        error("Unknown node type");
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

  void analyzeFunctionCall(FunctionCall* call){
  }

  void analyzeFunction(Function* function){
    Identifier* identifier = function->getIdentifier();
    string type = function->getType();

    auto result = symbolTable.emplace(identifier->getName(), Symbol("func", type, 0));
    checkIdentifierRedeclaration(result);
  }

  void analyzeAssignmentOperator(AssigmentOperator* assignmentOperator){
    Identifier* identifier = assignmentOperator->getIdentifier();
    Expression* value = assignmentOperator->getValue();

    if (!isIdentifierDeclared(identifier))
      error("Identifier: " + identifier->getName() + " is not declared");

    const string expectedType = symbolTable[identifier->getName()].type;
    checkExpressionType(expectedType, value);
  }

  bool isIdentifierDeclared(Identifier* identifier){
    return symbolTable.count(identifier->getName()) > 0;
  }

  void analyzeVariable(Variable* variable){
    string keyword = variable->getKeyword();
    string type = variable->getType();
    string identifier = variable->getName();
    Expression* value = variable->getValue();

    auto result = symbolTable.emplace(identifier, Symbol(keyword, type, 0));
    checkIdentifierRedeclaration(result);
    checkExpressionType(type, value);
  }

  void declareVariable(Variable* variable){
    
  }

  void checkIdentifierRedeclaration(const std::pair<unordered_map<string, Symbol>::iterator, bool>& result){
    if (!result.second){
      error("Identifier is already declared");
    }
  }

  void checkExpressionType(const string& expectedType, Expression* value) {
    string valueType = analyzeOperand(value);

    if (expectedType != valueType) {
      error("Type Mismatch: Expected " + expectedType + " but got " + valueType);
    }
  }

  string getIdentifierType(Identifier* identifier){
    const string name = identifier->getName();
    if (!isIdentifierDeclared(identifier))
      error("Identifier: " + name + " is not declared");
    return symbolTable[name].type;
  }

  string analyzeUnaryOperator(UnaryOperator* unaryOperator){
    Expression* operand = unaryOperator->getOperand();
    return analyzeOperand(operand);
  }

  string analyzeBinaryOperator(BinaryOperator* binaryOperator) {
    string leftType = analyzeOperand(binaryOperator->getLeftOperand());
    string rightType = analyzeOperand(binaryOperator->getRightOperand());

    if (leftType != rightType)
      error("Type Mismatch in binary operator: " + leftType + " and " + rightType + " do not match");

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
      error("Unknown operand type");
      return "";
    }
  }
};
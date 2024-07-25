#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "./ast.hpp"
#include "./scope.hpp"

using std::cout, std::endl;
using std::vector, std::string, std::unordered_map; 
using std::unique_ptr, std::make_unique;


class SemanticAnalyzer {
public:
  SemanticAnalyzer(const vector<unique_ptr<ASTNode>>& ast): 
    m_ast(std::move(ast)), m_scopes(make_unique<Scope>()) {}

  void analyze(){
    cout << "------ Semantics Start -----" << endl << endl;
    for(const unique_ptr<ASTNode>& node : m_ast){
      ASTNode* current = node.get();
      analyzeNode(current);
    }
    cout << endl << "------ Semantics End -----" << endl;
  }

private:
   const vector<unique_ptr<ASTNode>>& m_ast;
   unique_ptr<Scope> m_scopes;

  void error(const string& message){
    cout << message << endl;
    exit(1);
  }

  void analyzeNode(ASTNode* current) {
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
      analyzeIfStatement(statement);
    }

    else if (While* statement = dynamic_cast<While*>(current)){
      analyzeWhile(statement);
    }

    else if (Do* statement = dynamic_cast<Do*>(current)){
      analyzeDo(statement);
    }

    else if (For* statement = dynamic_cast<For*>(current)){
      analyzeFor(statement);
    }
    
    else {
      error("Unknown node type");
    }
  }

  void analyzeBody(BlockStatement* body) {
    for(ASTNode* current : body->getStatements()){
      analyzeNode(current);
    }
  }

  void analyzeFor(For* statement){

    Expression* condition = statement->getCondition();
    checkExpressionType("bool", condition);

    m_scopes->enterScope();

    BlockStatement* body = statement->getBody();
    analyzeBody(body);

    m_scopes->exitScope();
  }

  void analyzeDo(Do* statement){
    Expression* condition = statement->getCondition();
    checkExpressionType("bool", condition);

    m_scopes->enterScope();

    BlockStatement* body = statement->getBody();
    analyzeBody(body);

    m_scopes->exitScope();
  }

  void analyzeWhile(While* statement){
    Expression* condition = statement->getCondition();
    checkExpressionType("bool", condition);

    m_scopes->enterScope();

    BlockStatement* body = statement->getBody();
    analyzeBody(body);

    m_scopes->exitScope();
  }

  void analyzeIfStatement(IfStatement* statement){
    Expression* condition = statement->getCondition();
    checkExpressionType("bool", condition);

    m_scopes->enterScope();

    BlockStatement* body = statement->getBody();
    analyzeBody(body);

    m_scopes->exitScope();
  }

  void analyzeFunctionCall(FunctionCall* call){
    Identifier* identifier = call->getIdentifier();
    if (!m_scopes->isDeclared(call->getIdentifier()->getName()))
      error("Identifier: " + identifier->getName() + " is not declared");

    vector<Expression*> arguments = call->getArguments();
    const int n_arguments = arguments.size();

    vector<Parameter*> parameters = m_scopes->find(identifier->getName()).params;
    const int n_parameters = parameters.size();
    
    if (n_arguments != n_parameters)
      error("Error: Function " + identifier->getName() + " was expecting " + std::to_string(n_parameters) + " but instead got only " + std::to_string(n_arguments));
    
    for(int i = 0; i < n_parameters; i++){
      string expectedType = parameters[i]->getType();
      checkExpressionType(expectedType, arguments[i]);
    }
  }

  void checkReturnType(BlockStatement* body, string expectedType){
    vector<ASTNode*> statements = body->getStatements();
    for(const auto& statement: statements) {
      if (Return* returnValue = dynamic_cast<Return*>(statement)) {
        if (expectedType == "null")
          error("Return statement found in a function returning null");

        checkExpressionType(expectedType, returnValue->getValue());
        cout << "value match!";
      }
    }
  }

  void analyzeFunction(Function* function){
    Identifier* identifier = function->getIdentifier();
    string type = function->getType();
    vector<Parameter*> parameters = function->getParameters();
    BlockStatement* body = function->getBody();

    m_scopes->declare(identifier->getName(), Symbol("func", type, parameters));
    checkReturnType(body, type);

    m_scopes->enterScope();
    analyzeBody(body);
    m_scopes->exitScope();
  }

  void analyzeAssignmentOperator(AssigmentOperator* assignmentOperator){
    Identifier* identifier = assignmentOperator->getIdentifier();
    Expression* value = assignmentOperator->getValue();

    if (!m_scopes->isDeclared(identifier->getName()))
      error("Identifier: " + identifier->getName() + " is not declared");

    const string expectedType = m_scopes->find(identifier->getName()).type;
    checkExpressionType(expectedType, value);
  }

  void analyzeVariable(Variable* variable){
    string keyword = variable->getKeyword();
    string type = variable->getType();
    string name = variable->getName();
    Expression* value = variable->getValue();

    m_scopes->declare(name, Symbol(keyword, type));
    checkExpressionType(type, value);
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
    if (!m_scopes->isDeclared(name))
      error("Identifier: " + name + " is not declared");
    return m_scopes->find(name).type;
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
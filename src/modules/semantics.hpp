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
  string keyword; //var/const/func
  string type;
  int scopeLevel;

  Symbol() : keyword(""), type(""), scopeLevel(0) {} // Default constructor
  Symbol(string keyword, string type, int ScopeLevel): 
    keyword(keyword), type(type), scopeLevel(scopeLevel) {}
};

class SemanticAnalyzer {
public:
  SemanticAnalyzer(const vector<unique_ptr<ASTNode>>& ast): 
    m_ast(std::move(ast)){
  }

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
    checkVariableType(type, value);
  }

  void checkVariableRedeclaration(auto result){
    if (!result.second){
      error("Variable redeclaration");
    }
    else {
      cout << "Variable added!\n"; // remove later on
    }
  }

  void checkVariableType(const string& type, Expression* value){
    if (auto* str = dynamic_cast<String*>(value)){
      cout << "String yay!";
    }
    else if (auto* integer = dynamic_cast<Char*>(value)){
      cout << "Char yay";
    }
    else {
      cout << "other.";
    }
  }

};
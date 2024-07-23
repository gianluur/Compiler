#pragma once  

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>

#include "./ast.hpp"

using std::string, std::vector, std::stack, std::unordered_map;

struct Symbol {
  string keyword; // var/const/func
  string type;
  vector<Parameter*> params; // for function
  int scopeLevel;

  Symbol() : keyword(""), type(""), scopeLevel(0) {} // Default constructor
  Symbol(string keyword, string type, int scopeLevel): 
    keyword(keyword), type(type), scopeLevel(scopeLevel) {}

  Symbol(string keyword, string type, vector<Parameter*> parameters, int scopeLevel): 
    keyword(keyword), type(type), params(parameters), scopeLevel(scopeLevel) {}
};

class Scope {
public:
  Scope() {}

  void enter() {
    cout << "Enter scope";
  }
  void exit() {
    cout << "Exit scope";
  }
  void declare() {

  }
  void find() {

  }

  bool isDeclared() {
    
  }

private:
  stack<unordered_map<string, Symbol>> scopes;
};
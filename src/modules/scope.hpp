#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

using std::cout, std::vector, std::string, std::unordered_map;

struct Symbol {
  string keyword; // var/const/func
  string type;
  vector<Parameter*> params; // for function

  Symbol() : keyword(""), type("") {} // Default constructor
  Symbol(string keyword, string type): 
    keyword(keyword), type(type) {}

  Symbol(string keyword, string type, vector<Parameter*> parameters): 
    keyword(keyword), type(type), params(parameters) {}
};

class Scope {
public:
  Scope() { enterScope(); }
  ~Scope() {exitScope(); }

  void enterScope() {
    scopes.emplace_back();
  }

  void exitScope(){
    scopes.pop_back();
  }

  void declare(const string& name, const Symbol& symbol) {
    if (isRedeclared(name))
      error("Identifier: " + name + " is already declared");
    scopes.back().emplace(name, symbol);
  }

  bool isRedeclared(const string& name) const {
    return scopes.back().count(name) > 0;
  }

  bool isDeclared(const string& name) const {
    for (int i = scopes.size() - 1; i >= 0; i--){
      if (scopes[i].count(name) > 0) return true;
    }
    return false;
  }

  Symbol find(const string& name) const {
    for (int i = scopes.size() - 1; i >= 0; i--){
      if (scopes[i].count(name) > 0) return scopes[i].at(name);
    }
    error("Identifier: " + name + " is not declared");
    return Symbol();
  }
private:
  vector<unordered_map<string, Symbol>> scopes;

  void error(const string& message) const {
    cout << message << endl;
    exit(1);
  }

};
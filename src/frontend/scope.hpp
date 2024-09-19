#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <variant>
#include <memory>

#include "../includes/error.hpp"

using std::cout, std::vector, std::string, std::unordered_map, std::variant, std::unique_ptr;

struct Symbol {
  string keyword; // var/const/func
  string type;
  vector<Parameter*> params; // for function
  vector<Variable*> members; // for structs
  std::variant<string, Variable*, Function*, Struct*> node;

  Symbol() : keyword(""), type("") {}

  Symbol(string keyword, string type) :
    keyword(keyword), type(type) {}

  Symbol(string keyword, string type, vector<Parameter*> parameters) :
    keyword(keyword), type(type), params(parameters) {}

  Symbol(string keyword, string type, vector<Variable*> members) :
    keyword(keyword), type(type), members(members) {}

  Symbol(string keyword, string type, Variable* variable) :
    keyword(keyword), type(type), node(variable) {}

  Symbol(string keyword, string type, Function* function) :
    keyword(keyword), type(type), node(function) {}

  Symbol(string keyword, string type, Struct* structure) :
    keyword(keyword), type(type), node(structure) {}
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
      error("'" + name + "' is already declared");
    scopes.back().emplace(name, std::move(symbol));
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

  const Symbol& find(const string& name) const {
    for (int i = scopes.size() - 1; i >= 0; i--){
      if (scopes[i].count(name) > 0)
        return scopes[i].at(name);
    }
    error("'" + name + "'" + " is not declared");
    static Symbol dummy;
    return dummy;

  }

private:
  vector<unordered_map<string, Symbol>> scopes;
};
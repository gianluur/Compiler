#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <variant>
#include <memory>

#include "../includes/error.hpp"
#include "../includes/ASTNodeType.h"
// #include "../includes/nodes/variable.h"
// #include "../includes/nodes/struct.h"
// #include "../includes/nodes/function.h"

class Variable;
class Struct;
class Function;
class Parameter;

using std::cout, std::vector, std::string, std::unordered_map, std::variant, std::unique_ptr;

struct Symbol {
  ASTNodeType type;
  variant<string, const Variable*, const Function*, const Struct*> symbol;
  vector<variant<string, const Parameter*, const Variable*>> args;

  Symbol() {}

  Symbol(const Variable* variable) : symbol(variable) {}

  Symbol(const Function* function, const std::vector<Parameter*>& parameters) : symbol(function) {
    for (const Parameter* param : parameters) {
      args.emplace_back(param);
    }
  }

  Symbol(const Struct* structure, const std::vector<Variable*>& members) : symbol(structure) {
    for (const Variable* member : members) {
      args.emplace_back(member);
    }
  }
};

class Scope {
public:
  Scope() { enterScope(); }
  ~Scope() { exitScope(); }

  void enterScope() {
    symbolTable.emplace_back();
  }

  void exitScope() {
    symbolTable.pop_back();
  }

  void declare(const string& name, const Symbol& symbol) {
    if (isRedeclared(name))
      error("'" + name + "' is already declared");
    symbolTable.back().emplace(name, std::move(symbol));
  }

  bool isRedeclared(const string& name) const {
    return symbolTable.back().count(name) > 0;
  }

  bool isDeclared(const string& name) const {
    for (int i = symbolTable.size() - 1; i >= 0; i--)
      if (symbolTable[i].count(name) > 0) 
        return true;
    return false;
  }

  const Symbol& find(const string& name) const {
    for (int i = symbolTable.size() - 1; i >= 0; i--)
      if (symbolTable[i].count(name) > 0)
        return symbolTable[i].at(name);

    error("'" + name + "'" + " is not declared");
    static Symbol dummy;
    return dummy;

  }

private:
  vector<unordered_map<string, Symbol>> symbolTable;
};
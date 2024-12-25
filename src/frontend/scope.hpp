#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <variant>
#include <memory>

#include "../includes/error.hpp"
#include "../includes/ASTNodeType.h"

class Variable;
class Struct;
class Function;
class Parameter;

using std::cout, std::vector, std::string, std::unordered_map, std::variant, std::unique_ptr;
using std::make_unique;

struct Symbol {
  const ASTNodeType type;
  variant<string, const Variable*, const Function*, const Struct*> symbol;

  Symbol(): type(ASTNodeType::NULL) {}
  Symbol(const Variable* variable): type(ASTNodeType::VARIABLE), symbol(variable) {}
  Symbol(const Function* function): type(ASTNodeType::FUNCTION), symbol(function) {}
  Symbol(const Struct* structure): type(ASTNodeType::STRUCTURE), symbol(structure) {}
};

class Scope {
public:
  static Scope* getInstance();
  void enterScope();
  void exitScope();
  void declare(const string& name, const Symbol& symbol);
  bool isRedeclared(const string& name) const;
  bool isDeclared(const string& name) const;
  const Symbol& find(const string& name) const;

private:
  static Scope* instance;
  vector<unordered_map<string, Symbol>> symbolTable;
  size_t currentScope;

  Scope();  // Private constructor
};
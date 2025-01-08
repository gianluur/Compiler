#include "scope.hpp"

Scope* Scope::instance = nullptr;

Scope::Scope(): symbolTable(1), currentScope(0) {
  enterScope();
}

Scope* Scope::getInstance() {
  if (!instance) {
      instance = new Scope();
  }
  return instance;
}

void Scope::enterScope() {
  symbolTable.push_back(unordered_map<string, Symbol>());
  currentScope++;
}

void Scope::exitScope() {
  symbolTable.pop_back();
  currentScope--;
}

void Scope::declare(const string& name, const Symbol& symbol) {
  if (isRedeclared(name))
      error("'" + name + "' is already declared");
  symbolTable.back().emplace(name, std::move(symbol));
}

bool Scope::isRedeclared(const string& name) const {
  return symbolTable.back().count(name) > 0;
}

bool Scope::isDeclared(const string& name) const {
  for (int i = symbolTable.size() - 1; i >= 0; i--)
      if (symbolTable[i].count(name) > 0) 
          return true;
  return false;
}

const Symbol& Scope::find(const string& name, const bool quit) const {    
  for (int i = symbolTable.size() - 1; i >= 0; i--) {
      auto it = symbolTable[i].find(name);
      if (it != symbolTable[i].end())
          return it->second;
  }

  if (quit)
    error("'" + name + "'" + " is not declared");
  static Symbol dummy;
  return dummy;
}
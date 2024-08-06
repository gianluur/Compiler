#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

#include "llvm.hpp"
#include "../error.hpp"

using std::cout, std::vector, std::string, std::unordered_map;
using IRVariable = llvm::AllocaInst*;

//There's no need to check for declaration/redeclaration since the semantic analysis was already done
class IRScope {
public:
  IRScope() { enterScope(); }
  ~IRScope() { exitScope(); }

  void enterScope() {
    m_scopes.emplace_back();
  }

  void exitScope() {
    m_scopes.pop_back();
  }

  void declare(const string& name, IRVariable variable) {
    m_scopes.back().emplace(name, variable);
  }
  IRVariable find(const string& name) {
    for (int i = m_scopes.size() - 1; i >= 0; i--){
      if (m_scopes[i].count(name) > 0) return m_scopes[i].at(name);
    }
    return nullptr;
  }

  bool isGlobal(){
    return m_scopes.size() > 1;
  }

  

private:
  vector<unordered_map<string, IRVariable>> m_scopes;

};
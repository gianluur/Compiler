// #pragma once

// #include <iostream>
// #include <vector>
// #include <unordered_map>
// #include <string>

// #include "llvm.hpp"
// #include "../includes/error.hpp"

// using std::cout, std::vector, std::string, std::unordered_map;

// using IRVariable = llvm::AllocaInst*;
// using IRGlobalVariable = llvm::GlobalVariable*;
// using IRFunction = llvm::Function*;
// using IRStruct = llvm::StructType*;

// //There's no need to check for declaration/redeclaration since the semantic analysis was already done
// class IRScope {
// public:
//   IRScope() { enterScope(); }
//   ~IRScope() { exitScope(); }

//   void enterScope() {
//     m_varScopes.emplace_back();
//     m_funcScopes.emplace_back();
//     m_structScopes.emplace_back();
//   }

//   void exitScope() {
//     m_varScopes.pop_back();
//     m_funcScopes.pop_back();
//   }

//   void declareVariable(const string& name, IRVariable variable) {
//     m_varScopes.back().emplace(name, variable);
//   }
//   IRVariable findVariable(const string& name) {
//     for (int i = m_varScopes.size() - 1; i >= 0; i--){
//       if (m_varScopes[i].count(name) > 0) return m_varScopes[i].at(name);
//     }
//     return nullptr;
//   }

//   void declareGlobalVariable(const string& name, IRGlobalVariable globalVariable) {
//     m_globalVariables.emplace(name, globalVariable);
//   }
//   IRGlobalVariable findGlobalVariable(const string& name) const {
//     if (m_globalVariables.count(name) > 0) {
//       return m_globalVariables.at(name);
//     }
//     return nullptr;
//   }

//   void declareFunction(const string& name, IRFunction function) {
//     m_funcScopes.back().emplace(name, function);
//   }
//   IRFunction findFunction(const string& name) const {
//     for (int i = m_funcScopes.size() - 1; i >= 0; i--){
//       if (m_funcScopes[i].count(name) > 0) return m_funcScopes[i].at(name);
//     }
//     return nullptr;
//   }

//   void declareStruct(const string& name, std::pair<IRStruct, unordered_map<string, unsigned int>> structInfos){
//     m_structScopes.back().emplace(name, structInfos);
//   }

//   IRStruct findStruct(const string& name) const {
//     for (int i = m_structScopes.size() - 1; i >= 0; i--){
//       if (m_structScopes[i].count(name) > 0) return m_structScopes[i].at(name).first;
//     }
//     return nullptr;
//   }

//   unsigned int findStructMemberIndex(const IRStruct structure, const string& memberName) const {
//     for (int i = m_structScopes.size() - 1; i >= 0; i--){
//       for(const auto& [key, value]: m_structScopes[i]){
//         if (value.first == structure){
//           return value.second.at(memberName);
//         }
//       }
//     }
//     return -1;
//   }

// private:
//   vector<unordered_map<string, IRVariable>> m_varScopes;
//   vector<unordered_map<string, IRFunction>> m_funcScopes;
//   vector<unordered_map<string, std::pair<IRStruct, unordered_map<string, unsigned int>>>> m_structScopes;
//   unordered_map<string, IRGlobalVariable> m_globalVariables;
  
// };

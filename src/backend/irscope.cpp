#include "irscope.h"

IRScope::IRScope() { enterScope(); }
IRScope::~IRScope() { exitScope(); }

void IRScope::enterScope() {
  localVariableMaps.emplace_back();
  FunctionMaps.emplace_back();
  StructMaps.emplace_back();
}

void IRScope::exitScope() {
  if (!localVariableMaps.empty()) 
    localVariableMaps.pop_back();

  if (!FunctionMaps.empty()) 
    FunctionMaps.pop_back();
  
  if (!StructMaps.empty()) 
    StructMaps.pop_back();
}

void IRScope::declareVariable(const string& name, IRVariable variable) {
  localVariableMaps.back().emplace(name, variable);
}

optional<IRVariable> IRScope::findVariable(const string& name) const {
  for (int i = localVariableMaps.size() - 1; i >= 0; i--) {
    auto it = localVariableMaps[i].find(name);
    if (it != localVariableMaps[i].end()) 
      return it->second;
  }
  return std::nullopt;
}

void IRScope::declareGlobalVariable(const string& name, IRGlobalVariable globalVariable) {
  globalVariableMaps.emplace(name, globalVariable);
}

optional<IRGlobalVariable> IRScope::findGlobalVariable(const string& name) const {
  auto it = globalVariableMaps.find(name);
  if (it != globalVariableMaps.end()) 
    return it->second;
  return std::nullopt;
}

void IRScope::declareFunction(const string& name, IRFunction function) {
  FunctionMaps.back().emplace(name, function);
}

optional<IRFunction> IRScope::findFunction(const string& name) const {
  for (int i = FunctionMaps.size() - 1; i >= 0; i--) {
    auto it = FunctionMaps[i].find(name);
    if (it != FunctionMaps[i].end()) 
      return it->second;
  }
  return std::nullopt;
}

void IRScope::declareStruct(const string& name, std::pair<IRStruct, unordered_map<string, unsigned int>> structInfos) {
  StructMaps.back().emplace(name, std::move(structInfos));
}

optional<IRStruct> IRScope::findStruct(const string& name) const {
  for (int i = StructMaps.size() - 1; i >= 0; i--) {
    auto it = StructMaps[i].find(name);
    if (it != StructMaps[i].end()) 
      return it->second.first;
  }
  return std::nullopt;
}

optional<unsigned int> IRScope::findStructMemberIndex(const IRStruct structure, const string& memberName) const {
  for (int i = StructMaps.size() - 1; i >= 0; i--) {
    for (const auto& pair : StructMaps[i]) {

      if (pair.second.first == structure) {
        auto it = pair.second.second.find(memberName);
        if (it != pair.second.second.end()) {
          return it->second;
        }
      }
    }
  }
  return std::nullopt;
}
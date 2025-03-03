#pragma once

// C++ Headers
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>

// LLVM Headers
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

// Compiler Headers
#include "../includes/error.hpp"

// Using declarations
using std::vector, std::string, std::unordered_map, std::optional;
using IRVariable = llvm::AllocaInst*;
using IRGlobalVariable = llvm::GlobalVariable*;
using IRFunction = llvm::Function*;
using IRStruct = llvm::StructType*;

class IRScope {
public:

  // Constructor & Deconstructor
  IRScope();
  ~IRScope();

  // Scope modifiers
  void enterScope();
  void exitScope();

  // Global Variables
  void declareGlobalVariable(const string& name, IRGlobalVariable globalVariable);
  optional<IRGlobalVariable> findGlobalVariable(const string& name) const;

  // Local Variables
  void declareVariable(const string& name, IRVariable variable);
  optional<IRVariable> findVariable(const string& name) const;

  // Functions
  void declareFunction(const string& name, IRFunction function);
  optional<IRFunction> findFunction(const string& name) const;

  // Structs
  void declareStruct(const string& name, std::pair<IRStruct, unordered_map<string, unsigned int>> structInfos);
  optional<IRStruct> findStruct(const string& name) const;
  optional<unsigned int> findStructMemberIndex(const IRStruct structure, const string& memberName) const;

private:
  unordered_map<string, IRGlobalVariable> globalVariableMaps;
  vector<unordered_map<string, IRVariable>> localVariableMaps;
  vector<unordered_map<string, IRFunction>> FunctionMaps;
  vector<unordered_map<string, std::pair<IRStruct, unordered_map<string, unsigned int>>>> StructMaps;

};
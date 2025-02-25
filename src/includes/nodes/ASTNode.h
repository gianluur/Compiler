#pragma once

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include <cstddef>
#include <iostream>
#include <vector>
#include <iomanip>
#include <memory>
#include <variant>
#include <unordered_set>

#include "../../includes/token.hpp"
#include "../../frontend/scope.h"
#include "../ASTNodeType.h"

using std::cout, std::endl, std::setw;
using std::size_t, std::string, std::vector, std::unique_ptr, std::unordered_set, std::make_unique;

class Codegen;

class ASTNode {
public:
  ASTNode(ASTNodeType type): m_type(type) {}
  virtual ~ASTNode() = default;
  virtual void accept(Codegen* generator) const = 0;
  virtual void print(int indentation_level = 0) const = 0;
  ASTNodeType getNodeType() const { return m_type; }
  
private:
  ASTNodeType m_type;
};

#pragma once

#include <cstddef>
#include <iostream>
#include <vector>
#include <iomanip>
#include <memory>
#include <variant>
#include <unordered_set>
#include "../../includes/token.hpp"
#include "../../frontend/scope.hpp"
#include "../ASTNodeType.h"

// Common `using` declarations
using std::cout, std::endl, std::setw;
using std::size_t, std::string, std::vector, std::unique_ptr, std::unordered_set, std::make_unique;

class ASTNode {
public:
  ASTNode(ASTNodeType type): m_type(type) {}
  virtual ~ASTNode() = default;
  virtual void print(int indentation_level = 0) const = 0;
  ASTNodeType getNodeType() const { return m_type; }
private:
  ASTNodeType m_type;
};

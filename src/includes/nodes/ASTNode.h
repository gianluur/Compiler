#pragma once

#include <iostream>
#include <vector>
#include <iomanip>
#include <memory>
#include "../../includes/token.hpp"

// Common `using` declarations
using std::cout, std::endl, std::setw;
using std::size_t, std::string, std::vector, std::unique_ptr, std::make_unique;

class ASTNode {
public:
  virtual ~ASTNode() = default;
  virtual void print(int indentation_level = 0) const = 0;
};
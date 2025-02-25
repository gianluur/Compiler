#pragma once

#include <iostream>
#include "../lib/clistyle/clistyle.hpp"

using std::cerr, std::string;

[[noreturn]] inline void error(const string& message, const size_t line = 0) {
  cerr << CLIStyle::red << "Line: " << line << " | Error: " << CLIStyle::reset << message << "\n";
  exit(EXIT_FAILURE);
}

inline void warning(const string& message, const size_t line = 0) {
  cerr << "Line: " << line << " | Warning: " << message << "\n";
}

#pragma once

#include <iostream>
#include <string>

using std::cerr, std::string;

void error(const string& message, const int line = -1) {
  std::cerr << "Line " << line << " | Error: " << message << "\n";
  exit(EXIT_FAILURE);
}
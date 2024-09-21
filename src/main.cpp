#include <iostream>
#include <chrono>

#include "./frontend/preprocessing.hpp"  
#include "./frontend/tokenizer.hpp"
#include "./frontend/parser.hpp"
#include "./frontend/semantics.hpp"
#include "./includes/ast.hpp"
#include "./backend/codegen.hpp"

int main(int argc, char* argv[]){
  auto start = std::chrono::high_resolution_clock::now();

  Preprocessor preprocessed(argc, argv);
  Tokenizer tokenizer(preprocessed.getSrc());
  Parser parser(tokenizer.getTokens());
  Codegen codegen(parser.getAST());

  auto end = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  double seconds = std::chrono::duration<double>(duration).count();
  cout << "Compiling took: " << seconds << " seconds\n";

  return 0;
}
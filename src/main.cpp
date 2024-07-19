#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

#include "./modules/preprocessing.hpp"  
#include "./modules/tokenizer.hpp"
#include "./modules/parser.hpp"
#include "./modules/semantics.hpp"
#include "./modules/ast.hpp"

using std::cout, std::string, std::vector;
using std::unique_ptr;

int main(int argc, char* argv[]){
  auto start = std::chrono::high_resolution_clock::now();

  Preprocessor preprocessed(argc, argv);
  string src = preprocessed.getSrc();

  cout << src << endl << endl;

  Tokenizer tokenizer(src);
  vector<Token> tokens = tokenizer.getTokens();
  tokenizer.print();

  Parser parser(tokens);
  const vector<unique_ptr<ASTNode>>& ast = parser.getAST();

  SemanticAnalyzer analysis(ast);
  analysis.analyze();


  auto end = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  double seconds = std::chrono::duration<double>(duration).count();
  int milliseconds = duration.count() % 1000000 / 1000;
  cout << "Compiling took " << seconds << "seconds\n";

  return 0;
}
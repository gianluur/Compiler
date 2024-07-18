#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#include "./modules/preprocessing.hpp"  
#include "./modules/tokenizer.hpp"
#include "./modules/parser.hpp"

using std::cout, std::string, std::vector;

int main(int argc, char* argv[]){
  auto start = std::chrono::high_resolution_clock::now();

  Preprocessor preprocessed(argc, argv);
  string src = preprocessed.getSrc();

  cout << src << endl << endl;

  Tokenizer tokenizer(src);
  vector<Token> tokens = tokenizer.getTokens();
  tokenizer.print();

  // Parser parser(tokens);
  // parser.parse();

  auto end = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  double seconds = std::chrono::duration<double>(duration).count();
  int milliseconds = duration.count() % 1000000 / 1000;
  cout << "Compiling took " << seconds << "seconds\n";

  return 0;
}
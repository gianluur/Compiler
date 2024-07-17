#include <iostream>
#include <string>
#include <sstream>
#include <fstream>        
#include <algorithm>
#include <cctype>
#include <vector>

#include <chrono>

#include "./modules/preprocessing.hpp"  
#include "./modules/tokenizer/tokenizer.hpp"
#include "./modules/parser/parser.hpp"

using std::string, std::vector;

int main(int argc, char* argv[]){

  auto start = std::chrono::high_resolution_clock::now();

  Preprocessor preprocessed(argc, argv);
  string src = preprocessed.getSrc();

  Tokenizer tokenizer(src);
  vector<Token> tokens = tokenizer.getTokens();
  tokenizer.print();

  cout << endl;

  Parser parser(tokens);
  parser.parse();

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  double seconds = std::chrono::duration<double>(duration).count();
  int milliseconds = duration.count() % 1000000 / 1000;
  std::cout << "Compiling took " << seconds << "seconds\n";

  
  return EXIT_SUCCESS;
}
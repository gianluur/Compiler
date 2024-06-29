#include <iostream>
#include <string>
#include <sstream>
#include <fstream>        
#include <algorithm>
#include <cctype>
#include <vector>

#include "./modules/preprocessing.hpp"  
#include "./modules/tokenizer/tokenizer.hpp"
#include "./modules/parser/parser.hpp"

using std::string, std::vector;

int main(int argc, char* argv[]){
  
  Preprocessor preprocessed(argc, argv);
  string src = preprocessed.getSrc();

  Tokenizer tokenizer(src);
  vector<Token> tokens = tokenizer.getTokens();
  tokenizer.print();

  cout << endl;

  Parser parser(tokens);
  parser.parse();
  
  return EXIT_SUCCESS;
}
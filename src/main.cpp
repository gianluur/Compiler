#include <iostream>
#include <string>
#include <sstream>
#include <fstream>        
#include <algorithm>
#include <cctype>

#include <vector>

#include "./modules/preprocessing.hpp"  
#include "./modules/tokenizer.hpp"

using std::string, std::vector;

int main(int argc, char* argv[]){
  
  Preprocessor preprocessed(argc, argv);
  string src = preprocessed.getSrc();

  Tokenizer tokenizer(src);
  vector<Token> tokens = tokenizer.getTokens();
  

  return EXIT_SUCCESS;
}
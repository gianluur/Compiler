#pragma once

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
#include "./modules/codegen.hpp"

using std::cout, std::string, std::vector, std::unique_ptr;

class Compiler {
public:
  Compiler(int argc, char* argv[]) {
    Preprocessor preprocessed(argc, argv);
    const string src = preprocessed.getSrc();

    Tokenizer tokenizer(src);
    const vector<Token> tokens = tokenizer.getTokens();

    Parser parser(tokens);
    const vector<unique_ptr<ASTNode>>& raw_ast = parser.getAST();

    Semantics analysis(raw_ast);
    //const vector<unique_ptr<ASTNode>>& ast = analysis.getAST();
  }

private:
  unique_ptr<Preprocessor> preprocessor;
  unique_ptr<Tokenizer> tokenizer;
  unique_ptr<Parser> parser;
  unique_ptr<Semantics> semantics;
  //codegen
};

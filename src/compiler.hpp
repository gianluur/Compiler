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
    const vector<unique_ptr<ASTNode>>& ast = parser.getAST();
    
    Codegen codegen(ast);
  }
};

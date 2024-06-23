#include <iostream>
#include <memory>

#include "../ast/astnode.hpp"
#include "../ast/ast.hpp"

using std::cout, std::endl;
using std::unique_ptr, std::make_unique;

class Parser {
public:
  Parser(vector<Token> tokens): m_tokens(tokens) { parse(); }

  void print(){

  }

private:
  vector<Token> m_tokens;
  size_t i;

  void parse(){
    auto program = make_unique<Program>();
    for(; i <= m_tokens.size(); i++){
      program->statements.emplace_back();
    }
    program.get()->print();
  }

};
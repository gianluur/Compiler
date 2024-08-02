#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include "llvm.hpp"
#include "ast.hpp"
#include "error.hpp"

#include "IRGeneration/function.hpp"

using std::vector, std::unique_ptr, std::make_unique;

class Codegen {
public:
  Codegen(const vector<unique_ptr<ASTNode>>& ast): m_ast(ast), llvm(LLVM::getInstance()) { generateIR(); }

  void generateIR() {
    for (const unique_ptr<ASTNode>& node : m_ast) {
      ASTNode* current = node.get();

      if (Function* statement = dynamic_cast<Function*>(current)) {
        FuncGen function(statement);
      } 
      else 
        error("Current node isn't handled yet");
    }
    saveIRtoFile();
    printIR();
  }

private:
  const vector<unique_ptr<ASTNode>>& m_ast;
  LLVM& llvm;

  void printIR() const {
    std::cout << "----- IR Generator -----\n";
    llvm.module->print(llvm::errs(), nullptr);
    std::cout << "------------------------\n\n";
  }

  void saveIRtoFile() const {
    std::error_code errorCode;
    llvm::raw_fd_stream outFile("./src/build/out.ll", errorCode);
    llvm.module->print(outFile, nullptr);
  }
};

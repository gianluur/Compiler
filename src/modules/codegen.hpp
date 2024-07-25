#pragma once 

#include <iostream>
#include <vector>
#include <memory>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <llvm/IR/Verifier.h>

#include "./ast.hpp"

using std::vector, std::unique_ptr, std::make_unique;

class Codegen {
public:
    Codegen() : 
      context(), module("my_module", context) { setupMainFunction(); generateIR(); saveToFile("./src/build/out.ll"); }

    void setupMainFunction() {
      std::vector<llvm::Type*> mainFnArgs;
      mainFnArgs.push_back(llvm::Type::getInt32Ty(context));

      llvm::FunctionType *mainFnType = llvm::FunctionType::get(
          llvm::Type::getInt32Ty(context),
          mainFnArgs,
          false
      );

      mainFn = llvm::Function::Create(mainFnType, llvm::Function::ExternalLinkage, "main", &module);
      llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(context, "entry", mainFn);
      llvm::IRBuilder<> builder(entryBB);
      builder.CreateRet(builder.getInt32(1));
    }

    void generateIR() {
      if (llvm::verifyModule(module)) {
          llvm::errs() << "Failed to verify module!\n";
          return;
      }
      module.print(llvm::outs(), nullptr);
    }

    void saveToFile(const string& fileName){
      std::error_code errorCode;
      llvm::raw_fd_ostream outLL(fileName, errorCode);
      module.print(outLL, nullptr);
    }

private:
    llvm::LLVMContext context;
    llvm::Module module;
    llvm::Function *mainFn = nullptr;
};
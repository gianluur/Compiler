#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include "ast.hpp"
#include "error.hpp"

#include "./IRGeneration/llvm.hpp"
#include "./IRGeneration/scope.hpp"

using std::vector, std::unique_ptr, std::make_unique;

class Codegen {
public:
  Codegen(const vector<unique_ptr<ASTNode>>& ast): 
    m_ast(ast), llvm(LLVM::getInstance()) { generateIR(); }

  void generateIR() {
    llvm.scope.enterScope();
    for (const unique_ptr<ASTNode>& node : m_ast) {
      generateIRStatement(node.get());
    }
    llvm.scope.exitScope();

    saveIRtoFile();
    printIR();
  }

private:
  const vector<unique_ptr<ASTNode>>& m_ast;
  LLVM& llvm;
  
  void generateIRStatement(ASTNode* current, const bool isLocal = false){
    if (Function* statement = dynamic_cast<Function*>(current)) {
      generateFunction(statement);
    }
    else if (Variable* statement = dynamic_cast<Variable*>(current)){
      generateVariable(statement, isLocal);
    }
    else if (Return* statement = dynamic_cast<Return*>(current)){
      generateReturn(statement);
    }
    else 
      error("Current node isn't handled yet");
  }

  void generateFunction(Function* statement){
    const string name = statement->getIdentifier()->getName();
    llvm::Type* returnType = llvm.getLLVMType(statement->getType());
    vector<llvm::Type*> paramTypes = getParametersTypes(statement->getParameters());

    llvm::FunctionType* type = llvm::FunctionType::get(returnType, paramTypes, false);
    llvm::Function* function = llvm::Function::Create(type, llvm::Function::ExternalLinkage, name, llvm.module.get());
    llvm::BasicBlock* BB = llvm::BasicBlock::Create(llvm.context, "entry", function);
    llvm.builder.SetInsertPoint(BB);

    llvm.scope.enterScope();
    for(ASTNode* current: statement->getBody()->getStatements()){
      generateIRStatement(current, true);
    }
    llvm.scope.exitScope();

  }

  vector<llvm::Type*> getParametersTypes(const vector<Parameter*>& parameters) {
    vector<llvm::Type*> types;

    for (const Parameter* parameter : parameters) {
      llvm::Type* llvmType = llvm.getLLVMType(parameter->getType());
      types.emplace_back(llvmType);
    }
    return types;
  }

  void generateReturn(Return* node) {
    llvm::Value* value = llvm.getLLVMValue(node->getValue());
    llvm.builder.CreateRet(value);
  }

  void generateVariable(Variable* statement, const bool isLocal){
    isLocal ? generateLocalVariable(statement) : generateGlobalVariable(statement);
  }

  void generateLocalVariable(Variable* statement){
    const string name = statement->getName();
    llvm::Type* type = llvm.getLLVMType(statement->getType());

    llvm::AllocaInst* variable = llvm.builder.CreateAlloca(type, nullptr, name);

    if (!dynamic_cast<Null*>(statement->getValue())){
      llvm::Value* value = llvm.getLLVMValue(statement->getValue());
      llvm.builder.CreateStore(value, variable);
    }

    llvm.scope.declare(name, variable);
  }

  void generateGlobalVariable(Variable* statement){
    bool isConstant = (statement->getKeyword() == "const");
    llvm::Constant* value = llvm::cast<llvm::Constant>(llvm.getLLVMValue(statement->getValue()));
    llvm::Type* type = llvm.getLLVMType(statement->getType());
    new llvm::GlobalVariable(*llvm.module, type, isConstant, llvm::GlobalValue::ExternalLinkage, value, statement->getName());
  }

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

#pragma once 

#include <iostream>
#include <vector>
#include <memory>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

#include "ast.hpp"
#include "error.hpp"

using std::vector, std::unique_ptr, std::make_unique;

class Codegen {
public:
  Codegen(const vector<unique_ptr<ASTNode>>& ast): 
    m_ast(std::move(ast)), context(), module("MyModule", context), builder(context) { generateIR(); }

  void generateIR(){
    for (const unique_ptr<ASTNode>& node: m_ast){
      ASTNode* current = node.get();

      if (Function* statement = dynamic_cast<Function*>(current)){
        generateFunction(statement);
      }
      else 
        error("Current node isn't handled yet");
    }
    saveIR();
    printIR();
  }

private:
  const vector<unique_ptr<ASTNode>>& m_ast;

    llvm::LLVMContext context;
    llvm::Module module;
    llvm::IRBuilder<> builder;

  void printIR() const {
    cout << "----- IR Generator -----\n";
    module.print(llvm::errs(), nullptr);
    cout << "------------------------\n\n";
  }

  void saveIR() const {
    std::error_code errorCode;
    llvm::raw_fd_stream outFile("./src/build/out.ll", errorCode);
    module.print(outFile, nullptr);
  }

  //missing string type
  llvm::Type* getLLVMType(const string& type) {
    if (type == "int"){
      return llvm::Type::getInt32Ty(context);
    }
    else if (type == "float"){
      return llvm::Type::getFloatTy(context);
    }
    else if (type == "char"){
      return llvm::Type::getInt8Ty(context);
    }
    else if (type == "bool"){
      return llvm::Type::getInt1Ty(context);
    }
    else {
      return llvm::Type::getVoidTy(context);
    }
  }

  void generateFunction(Function* node) {
    const std::string& nodeType = node->getType();
    const std::string& nodeName = node->getIdentifier()->getName();
    const std::vector<Parameter*> nodeParameters = node->getParameters();
    const std::vector<ASTNode*> nodeBody = node->getBody()->getStatements();

    std::vector<llvm::Type*> parameterTypes;

    for (const Parameter* parameter : nodeParameters) {
      const std::string paramType = parameter->getType();
      llvm::Type* llvmType = getLLVMType(paramType);
      parameterTypes.emplace_back(llvmType);
    }

    llvm::FunctionType* funcType = llvm::FunctionType::get(getLLVMType(nodeType), parameterTypes, false);
    llvm::Function* function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, nodeName, module);
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", function);
    builder.SetInsertPoint(entry);

    for (ASTNode* current : nodeBody) {
      if (Return* returnNode = dynamic_cast<Return*>(current)) {
        Expression* value = returnNode->getValue();
        llvm::Value* returnValue = nullptr;

        if (Integer* intValue = dynamic_cast<Integer*>(value)) {
          returnValue = builder.getInt32(std::stoi(intValue->getValue()));
        } 
        else if (Float* floatValue = dynamic_cast<Float*>(value)) {
          returnValue = llvm::ConstantFP::get(context, llvm::APFloat(std::stof(floatValue->getValue())));
        } 
        else if (Char* charValue = dynamic_cast<Char*>(value)) {
          returnValue = builder.getInt8(static_cast<int>(charValue->getValue()[0]));
        } 
        else if (Boolean* boolValue = dynamic_cast<Boolean*>(value)) {
          returnValue = builder.getInt1(boolValue->getValue() == "true");
        } 
        else if (dynamic_cast<Null*>(value)) {
          returnValue = llvm::Constant::getNullValue(getLLVMType("null"));
        } 
        else {
          error("Can't convert return value type");
        }
        
        builder.CreateRet(returnValue);
      }
    }
    llvm::verifyFunction(*function);
  }
};
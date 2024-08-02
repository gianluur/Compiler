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

//I'm using the singleton pattern because i want to pass this class to a lot of class without reinitializing the members everytime
class LLVM {
public:
  static LLVM& getInstance() {
    static LLVM instance;
    return instance;
  }

  llvm::LLVMContext context;
  std::unique_ptr<llvm::Module> module;
  llvm::IRBuilder<> builder;

  llvm::Type* getLLVMType(const std::string& type) {
    if (type == "int") {
      return llvm::Type::getInt32Ty(context);
    } else if (type == "float") {
      return llvm::Type::getFloatTy(context);
    } else if (type == "char") {
      return llvm::Type::getInt8Ty(context);
    } else if (type == "bool") {
      return llvm::Type::getInt1Ty(context);
    } else {
      return llvm::Type::getVoidTy(context);
    }
  }

private:
  LLVM(): context(), module(std::make_unique<llvm::Module>("MyModule", context)), builder(context) {}

  // Delete copy constructor and assignment operator to ensure singleton
  LLVM(const LLVM&) = delete;
  LLVM& operator=(const LLVM&) = delete;
};

class FuncGen {
public:
  FuncGen(Function* node)
    : m_name(node->getIdentifier()->getName()), m_type(node->getType()),
      m_parameters(node->getParameters()), m_body(node->getBody()->getStatements()), llvm(LLVM::getInstance()) {

    vector<llvm::Type*> paramTypes = getParametersTypes(m_parameters);

    llvm::FunctionType* funcType = llvm::FunctionType::get(llvm.getLLVMType(m_type), paramTypes, false);
    llvm::Function* function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, m_name, llvm.module.get());
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(llvm.context, "entry", function);
    llvm.builder.SetInsertPoint(entry);

    for (ASTNode* current : m_body) {
      if (Return* returnNode = dynamic_cast<Return*>(current)) {
        generateReturnValue(returnNode);
      }
    }
    llvm::verifyFunction(*function);
  }

private:
  std::string m_name;
  std::string m_type;
  vector<Parameter*> m_parameters;
  vector<ASTNode*> m_body;
  LLVM& llvm;

  void generateReturnValue(Return* node) {
    Expression* value = node->getValue();
    llvm::Value* returnValue = nullptr;

    if (Integer* intValue = dynamic_cast<Integer*>(value)) 
      returnValue = llvm.builder.getInt32(std::stoi(intValue->getValue()));
    
    else if (Float* floatValue = dynamic_cast<Float*>(value)) 
      returnValue = llvm::ConstantFP::get(llvm.context, llvm::APFloat(std::stof(floatValue->getValue())));
    
    else if (Char* charValue = dynamic_cast<Char*>(value)) 
      returnValue = llvm.builder.getInt8(static_cast<int>(charValue->getValue()[0]));
    
    else if (Boolean* boolValue = dynamic_cast<Boolean*>(value)) 
      returnValue = llvm.builder.getInt1(boolValue->getValue() == "true");
    
    else if (dynamic_cast<Null*>(value)) 
      returnValue = llvm::Constant::getNullValue(llvm.getLLVMType("null"));
    
    else 
      error("Can't convert return value type");
    
    llvm.builder.CreateRet(returnValue);
  }

  vector<llvm::Type*> getParametersTypes(const vector<Parameter*>& parameters) {
    vector<llvm::Type*> types;

    for (const Parameter* parameter : parameters) {
      const std::string paramType = parameter->getType();
      llvm::Type* llvmType = llvm.getLLVMType(paramType);
      types.emplace_back(llvmType);
    }

    return types;
  }
};


class Codegen {
public:
  Codegen(const vector<unique_ptr<ASTNode>>& ast)
    : m_ast(ast), llvm(LLVM::getInstance()) {
    generateIR();
  }

  void generateIR() {
    for (const unique_ptr<ASTNode>& node : m_ast) {
      ASTNode* current = node.get();

      if (Function* statement = dynamic_cast<Function*>(current)) {
        FuncGen function(statement);
      } else {
        error("Current node isn't handled yet");
      }
    }
    saveIR();
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

  void saveIR() const {
    std::error_code errorCode;
    llvm::raw_fd_stream outFile("./src/build/out.ll", errorCode);
    llvm.module->print(outFile, nullptr);
  }
};

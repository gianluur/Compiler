#pragma once  

#include <iostream>
#include <vector>
#include <memory>

#include "../llvm.hpp"
#include "../ast.hpp"
#include "../error.hpp"

class FuncGen {
public:
  FuncGen(Function* node):
    m_name(node->getIdentifier()->getName()), m_type(node->getType()),
    m_parameters(node->getParameters()), m_body(node->getBody()->getStatements()), 
    llvm(LLVM::getInstance()) { generate(); }

private:
  string m_name;
  string m_type;
  vector<Parameter*> m_parameters;
  vector<ASTNode*> m_body;
  LLVM& llvm;

  void generate(){
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

  void generateReturnValue(Return* node) {
    llvm::Value* value = llvm.getLLVMValue(node->getValue());
    llvm.builder.CreateRet(value);
  }

  vector<llvm::Type*> getParametersTypes(const vector<Parameter*>& parameters) {
    vector<llvm::Type*> types;

    for (const Parameter* parameter : parameters) {
      llvm::Type* llvmType = llvm.getLLVMType(parameter->getType());
      types.emplace_back(llvmType);
    }
    return types;
  }
};
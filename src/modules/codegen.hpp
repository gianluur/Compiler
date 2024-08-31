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
    else if (IfStatement* statement = dynamic_cast<IfStatement*>(current)){
      generateIfStatement(statement);
    }
    else if (AssigmentOperator* statement = dynamic_cast<AssigmentOperator*>(current)){
      generateAssignmentOperator(statement);
    }
    // else if (FunctionCall* statement = dynamic_cast<FunctionCall*>(statement)){
    //   generateFunctionCall(statement);
    // }
    else 
      error("Current node isn't handled yet");
  }

  void generateAssignmentOperator(AssigmentOperator* statement){
    string op = statement->getOperator();

    llvm::AllocaInst* variable = llvm.scope.findVariable(statement->getIdentifier()->getName());
    llvm::Type* variableType = variable->getAllocatedType();

    llvm::Value* assignmentValue = llvm.getLLVMValue(statement->getValue());
    llvm::Value* currentValue = llvm.builder.CreateLoad(variableType, variable, "currentValue");
    llvm::Value* result;


    if (op == "=") {
      result = assignmentValue;
    } 
    else {
      bool isFloat = variableType->isFloatTy();

      if (op == "+=") {
        result = isFloat ? llvm.builder.CreateFAdd(currentValue, assignmentValue, "addtmp") 
                         : llvm.builder.CreateAdd(currentValue, assignmentValue, "addtmp");
      } else if (op == "-=") {
        result = isFloat ? llvm.builder.CreateFSub(currentValue, assignmentValue, "subtmp") 
                         : llvm.builder.CreateSub(currentValue, assignmentValue, "subtmp");
      } else if (op == "*=") {
        result = isFloat ? llvm.builder.CreateFMul(currentValue, assignmentValue, "multmp") 
                         : llvm.builder.CreateMul(currentValue, assignmentValue, "multmp");
      } else if (op == "/=") {
        result = isFloat ? llvm.builder.CreateFDiv(currentValue, assignmentValue, "divtmp") 
                         : llvm.builder.CreateSDiv(currentValue, assignmentValue, "divtmp");
      } else if (op == "%=") {
        result = isFloat ? llvm.builder.CreateFRem(currentValue, assignmentValue, "modtmp") 
                         : llvm.builder.CreateSRem(currentValue, assignmentValue, "modtmp");
      } else {
        error("Unsupported assignment operator: " + op);
        return;
      }
    }
    llvm.builder.CreateStore(result, variable);
 
  }

  void generateIfStatement(IfStatement* statement){
    llvm::Value* condition = llvm.getLLVMValue(statement->getCondition());
    if (!condition) {
      error("Failed to generate condition for if statement");
      return;
    }

    llvm::Function* currentFunction = llvm.builder.GetInsertBlock()->getParent();
    if (!currentFunction) {
      error("If statement must be inside a function");
      return;
    }

    llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(llvm.context, "then", currentFunction);
    llvm::BasicBlock* elseBlock = llvm::BasicBlock::Create(llvm.context, "else");
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(llvm.context, "ifcont");

    llvm.builder.CreateCondBr(condition, thenBlock, elseBlock);

    llvm.builder.SetInsertPoint(thenBlock);
    llvm.scope.enterScope();
    for (ASTNode* current : statement->getBody()->getStatements()) {
      generateIRStatement(current);
    }
    llvm.scope.exitScope();
    llvm.builder.CreateBr(mergeBlock);
    
    thenBlock = llvm.builder.GetInsertBlock();

    currentFunction->insert(currentFunction->end(), elseBlock);
    llvm.builder.SetInsertPoint(elseBlock);
    
    if (statement->getElseStatement()) {
      llvm.scope.enterScope();
      for (ASTNode* current : statement->getElseStatement()->getBody()->getStatements()) {
        generateIRStatement(current);
      }
      llvm.scope.exitScope();
    }
    llvm.builder.CreateBr(mergeBlock);
    
    elseBlock = llvm.builder.GetInsertBlock();

    currentFunction->insert(currentFunction->end(), mergeBlock);
    llvm.builder.SetInsertPoint(mergeBlock);
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

    if (name != "main")
      llvm.scope.declareFunction(name, function);
    
    llvm::verifyFunction(*function);
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

    llvm.scope.declareVariable(name, variable);
  }

  void generateGlobalVariable(Variable* statement){
    bool isConstant = (statement->getKeyword() == "const");
    llvm::Constant* value = llvm::cast<llvm::Constant>(llvm.getLLVMValue(statement->getValue()));
    llvm::Type* type = llvm.getLLVMType(statement->getType());
    unique_ptr<llvm::GlobalVariable> globalVariable = std::make_unique<llvm::GlobalVariable>(*llvm.module, type, isConstant, llvm::GlobalValue::ExternalLinkage, value, statement->getName());
    llvm.scope.declareGlobalVariable(statement->getName(), globalVariable.get());
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

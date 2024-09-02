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
    else if (While* statement = dynamic_cast<While*>(current)){
      generateWhileStatement(statement);
    }
    else if (Do* statement = dynamic_cast<Do*>(current)){
      generateDoWhileStatement(statement);
    }
    else if (For* statement = dynamic_cast<For*>(current)){
      generateForStatement(statement);
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

  void generateForStatement(For* statement){
    llvm::Function* currentFunction = llvm.builder.GetInsertBlock()->getParent();
    if (!currentFunction) {
        error("For loop must be inside a function");
        return;
    }

    // Create basic blocks for the loop
    llvm::BasicBlock* initBlock = llvm::BasicBlock::Create(llvm.context, "for.init", currentFunction);
    llvm::BasicBlock* conditionBlock = llvm::BasicBlock::Create(llvm.context, "for.cond");
    llvm::BasicBlock* loopBlock = llvm::BasicBlock::Create(llvm.context, "for.body");
    llvm::BasicBlock* updateBlock = llvm::BasicBlock::Create(llvm.context, "for.update");
    llvm::BasicBlock* afterLoopBlock = llvm::BasicBlock::Create(llvm.context, "for.end");

    // Branch to the initialization block
    llvm.builder.CreateBr(initBlock);

    // Generate initialization code
    llvm.builder.SetInsertPoint(initBlock);
    llvm.scope.enterScope(); // New scope for loop variable
    if (statement->getInitialization()) {
      generateIRStatement(statement->getInitialization(), true);
    }
    llvm.builder.CreateBr(conditionBlock);

    // Generate condition code
    currentFunction->insert(currentFunction->end(), conditionBlock);
    llvm.builder.SetInsertPoint(conditionBlock);
    llvm::Value* condition = llvm.getLLVMValue(statement->getCondition());
    llvm.builder.CreateCondBr(condition, loopBlock, afterLoopBlock);
    cout << "this one2\n";

    // Generate loop body
    currentFunction->insert(currentFunction->end(), loopBlock);
    llvm.builder.SetInsertPoint(loopBlock);
    for (ASTNode* current : statement->getBody()->getStatements()) {
        generateIRStatement(current);
    }
    llvm.builder.CreateBr(updateBlock);

    // Generate update code
    currentFunction->insert(currentFunction->end(), updateBlock);
    llvm.builder.SetInsertPoint(updateBlock);
    if (statement->getUpdate()) {
        generateIRStatement(statement->getUpdate());
    }
    llvm.builder.CreateBr(conditionBlock);

    // Set insert point to the block after the loop
    currentFunction->insert(currentFunction->end(), afterLoopBlock);
    llvm.builder.SetInsertPoint(afterLoopBlock);
    llvm.scope.exitScope(); // Exit the loop variable scope
  }

  void generateDoWhileStatement(Do* statement){
    llvm::Function* currentFunction = llvm.builder.GetInsertBlock()->getParent();
    if (!currentFunction) {
      error("Do-while statement must be inside a function");
      return;
    }

    // Create basic blocks for the loop
    llvm::BasicBlock* loopBlock = llvm::BasicBlock::Create(llvm.context, "dowhile.body", currentFunction);
    llvm::BasicBlock* conditionBlock = llvm::BasicBlock::Create(llvm.context, "dowhile.cond");
    llvm::BasicBlock* afterLoopBlock = llvm::BasicBlock::Create(llvm.context, "dowhile.end");

    // Branch to the loop body
    llvm.builder.CreateBr(loopBlock);

    // Generate loop body
    llvm.builder.SetInsertPoint(loopBlock);
    llvm.scope.enterScope();
    for (ASTNode* current : statement->getBody()->getStatements()) {
      generateIRStatement(current);
    }
    llvm.scope.exitScope();

    // Branch to the condition block
    llvm.builder.CreateBr(conditionBlock);

    // Generate condition code
    currentFunction->insert(currentFunction->end(), conditionBlock);
    llvm.builder.SetInsertPoint(conditionBlock);
    llvm::Value* condition = llvm.getLLVMValue(statement->getCondition());
    llvm.builder.CreateCondBr(condition, loopBlock, afterLoopBlock);

    // Set insert point to the block after the loop
    currentFunction->insert(currentFunction->end(), afterLoopBlock);
    llvm.builder.SetInsertPoint(afterLoopBlock);
  }

  void generateWhileStatement(While* statement){
    llvm::Function* currentFunction = llvm.builder.GetInsertBlock()->getParent();
    if (!currentFunction) {
        error("While statement must be inside a function");
        return;
    }

    llvm::BasicBlock* conditionBlock = llvm::BasicBlock::Create(llvm.context, "while.cond", currentFunction);
    llvm::BasicBlock* loopBlock = llvm::BasicBlock::Create(llvm.context, "while.body");
    llvm::BasicBlock* afterLoopBlock = llvm::BasicBlock::Create(llvm.context, "while.end");

    llvm.builder.CreateBr(conditionBlock);

    llvm.builder.SetInsertPoint(conditionBlock);

    llvm::Value* condition = llvm.getLLVMValue(statement->getCondition());
    llvm.builder.CreateCondBr(condition, loopBlock, afterLoopBlock);

    currentFunction->insert(currentFunction->end(), loopBlock);
    llvm.builder.SetInsertPoint(loopBlock);

    llvm.scope.enterScope();
    for (ASTNode* current : statement->getBody()->getStatements()) {
        generateIRStatement(current);
    }
    llvm.scope.exitScope();

    llvm.builder.CreateBr(conditionBlock);

    currentFunction->insert(currentFunction->end(), afterLoopBlock);
    llvm.builder.SetInsertPoint(afterLoopBlock);
  }

  void generateIfStatement(IfStatement* statement){
    llvm::Function* currentFunction = llvm.builder.GetInsertBlock()->getParent();
      if (!currentFunction) {
        error("If statement must be inside a function");
        return;
      }

      llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(llvm.context, "ifcont");
      
      // Generate initial 'if' condition and block
      llvm::Value* condition = llvm.getLLVMValue(statement->getCondition());

      llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(llvm.context, "then", currentFunction);
      llvm::BasicBlock* nextBlock = llvm::BasicBlock::Create(llvm.context, "elseif");

      llvm.builder.CreateCondBr(condition, thenBlock, nextBlock);

      // Generate 'then' block
      llvm.builder.SetInsertPoint(thenBlock);
      llvm.scope.enterScope();
      for (ASTNode* current : statement->getBody()->getStatements()) {
        generateIRStatement(current);
      }
      llvm.scope.exitScope();
      llvm.builder.CreateBr(mergeBlock);

      // Handle 'else if' statements
      llvm::BasicBlock* currentBlock = nextBlock;
      vector<ElseIfStatement*> elseIfStatements = statement->getElseIfStatements();
      
      for (size_t i = 0; i < elseIfStatements.size(); ++i) {
        currentFunction->insert(currentFunction->end(), currentBlock);
        llvm.builder.SetInsertPoint(currentBlock);

        ElseIfStatement* elseIf = elseIfStatements[i];
        condition = llvm.getLLVMValue(elseIf->getCondition());
        if (!condition) {
          error("Failed to generate condition for else-if statement");
          return;
        }

        thenBlock = llvm::BasicBlock::Create(llvm.context, "elseif_then", currentFunction);
        nextBlock = (i == elseIfStatements.size() - 1) ? 
                    llvm::BasicBlock::Create(llvm.context, "else") :
                    llvm::BasicBlock::Create(llvm.context, "elseif_next");

        llvm.builder.CreateCondBr(condition, thenBlock, nextBlock);

        // Generate 'else if' block
        llvm.builder.SetInsertPoint(thenBlock);
        llvm.scope.enterScope();
        for (ASTNode* current : elseIf->getBody()->getStatements()) {
          generateIRStatement(current);
        }
        llvm.scope.exitScope();
        llvm.builder.CreateBr(mergeBlock);

        currentBlock = nextBlock;
      }

      // Handle final 'else' block
      currentFunction->insert(currentFunction->end(), currentBlock);
      llvm.builder.SetInsertPoint(currentBlock);

      if (statement->getElseStatement()) {
        llvm.scope.enterScope();
        for (ASTNode* current : statement->getElseStatement()->getBody()->getStatements()) {
          generateIRStatement(current);
        }
        llvm.scope.exitScope();
      }
      llvm.builder.CreateBr(mergeBlock);

      // Set insert point to merge block
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

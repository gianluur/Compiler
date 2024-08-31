#pragma once

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

#include "scope.hpp"
#include "../error.hpp"

// I'm using the singleton pattern because i want to pass this class to a lot of class without reinitializing the members everytime
class LLVM {
public:
  static LLVM& getInstance() {
    static LLVM instance;
    return instance;
  }

  llvm::LLVMContext context;
  std::unique_ptr<llvm::Module> module;
  llvm::IRBuilder<> builder;
  IRScope scope;

  llvm::Type* getLLVMType(const string& type) {
    if (type == "int") return llvm::Type::getInt32Ty(context);
    else if (type == "float") return llvm::Type::getFloatTy(context);
    else if (type == "char") return llvm::Type::getInt8Ty(context);
    else if (type == "bool") return llvm::Type::getInt1Ty(context);
    else return llvm::Type::getVoidTy(context);
  }

  llvm::Value* getLLVMValue(Expression* node){
    if (Integer* statement = dynamic_cast<Integer*>(node)) 
      return builder.getInt32(std::stoi(statement->getValue()));

    else if (Float* statement = dynamic_cast<Float*>(node)) 
      return llvm::ConstantFP::get(llvm::Type::getFloatTy(context), std::stod(statement->getValue()));
    
    else if (Char* statement = dynamic_cast<Char*>(node)){
      return builder.getInt8(static_cast<int>(statement->getValue()[1]));} // ...getValue()[1] <-- Ex. the char 'A' is at pos 1, true for all chars
    
    else if (Boolean* statement = dynamic_cast<Boolean*>(node)) 
      return builder.getInt1(statement->getValue() == "true");
    
    else if (BinaryOperator* statement = dynamic_cast<BinaryOperator*>(node)) 
      return generateBinaryOperator(statement);
    
    else if (UnaryOperator* statement = dynamic_cast<UnaryOperator*>(node)){
      return generateUnaryOperator(statement);
    }
    else if (Identifier* statement = dynamic_cast<Identifier*>(node)){
      return generateIdentifier(statement);
    }
    else if (Cast* statement = dynamic_cast<Cast*>(node))
      return generateCast(statement);

    else if (FunctionCall* statement = dynamic_cast<FunctionCall*>(node)){
      return generateFunctionCall(statement);
    }

    else if (dynamic_cast<Null*>(node)) 
      return llvm::Constant::getNullValue(getLLVMType("null"));

    else {
      error("Can't convert expression to an valid LLVM value");
      return nullptr;
    }
  }

  llvm::Value* generateFunctionCall(FunctionCall* statement){
    const string name = statement->getIdentifier()->getName();
    llvm::Function* calledFunction = scope.findFunction(name);
    vector<llvm::Value*> argumentsValues = getArgumentsValue(statement->getArguments());
    llvm::Value* call = builder.CreateCall(calledFunction, argumentsValues, name + "_call");

    if (calledFunction->getReturnType()->isVoidTy())
      return nullptr;
    else
      return call;
  }

  vector<llvm::Value*> getArgumentsValue(const vector<Expression*>& arguments){
    vector<llvm::Value*> argumentsValues;
    for (Expression* argNode : arguments) {
      llvm::Value* value = getLLVMValue(argNode);
      argumentsValues.push_back(value);
    }
    return argumentsValues;
  }
  
  llvm::Value* generateIdentifier(Identifier* statement){
    const string name = statement->getName();
    
    llvm::AllocaInst* localVariable = scope.findVariable(name);
    if (localVariable)
      return builder.CreateLoad(localVariable->getAllocatedType(), localVariable, name.c_str());
  

    // add global variable

    error("Could not generate the correct value from identifier: " + name);
    return nullptr;
  }

  llvm::Value* generateCast(Cast* statement){
    llvm::Value* value = getLLVMValue(statement->getExpression());
    llvm::Type* targetType = getLLVMType(statement->getTargetType());
    llvm::Type* actualType = value->getType();

    if (actualType->isIntegerTy() && targetType->isFloatTy())
      return builder.CreateSIToFP(value, targetType, "intToFloat");

    else if (actualType->isFloatTy() && targetType == llvm::Type::getInt32Ty(context))
      return builder.CreateFPToSI(value, targetType, "floatToInt");

    else if (actualType == llvm::Type::getInt8Ty(context) && targetType == llvm::Type::getInt32Ty(context))
      return builder.CreateZExt(value, targetType, "charToInt");

    else if (actualType == llvm::Type::getInt32Ty(context) && targetType == llvm::Type::getInt8Ty(context))
      return builder.CreateTrunc(value, targetType, "intToChar");

    else if (actualType->isIntegerTy() && targetType == llvm::Type::getInt1Ty(context))
      return builder.CreateICmpNE(value, llvm::ConstantInt::get(actualType, 0), "intToBool");
    
    else if (actualType == llvm::Type::getInt1Ty(context) && targetType->isIntegerTy())
      return builder.CreateZExt(value, targetType, "boolToInt");
    
    else if (actualType->isFloatTy() && targetType == llvm::Type::getInt1Ty(context))
      return builder.CreateFCmpONE(value, llvm::ConstantFP::get(actualType, 0.0), "floatToBool");
    
    else {
      error("Couldn't generate a cast expression");
      return nullptr;
    }
  }

  llvm::Value* generateUnaryOperator(UnaryOperator* statement){
    llvm::Value* value = getLLVMValue(statement->getOperand());
    llvm::Type* type = value->getType();

    if (type == llvm::Type::getInt1Ty(context)) { // Check if the type is boolean
      // For boolean, we use icmp to compare with 0 and then zext to ensure the result is a proper boolean
      return builder.CreateICmpEQ(value, llvm::ConstantInt::get(type, 0));
    }
    else {
      error("Couldn't generate unary expression");
      return nullptr;
    }
  }

  llvm::Value* generateBinaryOperator(BinaryOperator* statement) {
    llvm::Value* leftValue = getLLVMValue(statement->getLeftOperand());
    llvm::Value* rightValue = getLLVMValue(statement->getRightOperand());
    const string op = statement->getOperator();
    
    if (op == "+"){
      if (leftValue->getType()->isFloatTy() && rightValue->getType()->isFloatTy()) 
        return builder.CreateFAdd(leftValue, rightValue, "faddtmp");
      else 
        return builder.CreateAdd(leftValue, rightValue, "addtmp");
    }

    else if (op == "-") 
      if (leftValue->getType()->isFloatTy() && rightValue->getType()->isFloatTy()) 
        return builder.CreateFSub(leftValue, rightValue, "faddtmp");
      else 
        return builder.CreateSub(leftValue, rightValue, "addtmp");

    else if (op == "*") 
      if (leftValue->getType()->isFloatTy() && rightValue->getType()->isFloatTy()) 
        return builder.CreateFMul(leftValue, rightValue, "faddtmp");
      else 
        return builder.CreateMul(leftValue, rightValue, "addtmp");

    else if (op == "/") 
      if (leftValue->getType()->isFloatTy() && rightValue->getType()->isFloatTy()) 
        return builder.CreateFDiv(leftValue, rightValue, "faddtmp");
      else 
        return builder.CreateSDiv(leftValue, rightValue, "addtmp");

    else if (op == "%")
      if (leftValue->getType()->isFloatTy() && rightValue->getType()->isFloatTy()) 
        return builder.CreateFRem(leftValue, rightValue, "faddtmp");
      else 
        return builder.CreateSRem(leftValue, rightValue, "addtmp");

    else if (op == "==") 
      if (leftValue->getType()->isFloatTy() && rightValue->getType()->isFloatTy()) 
        return builder.CreateFCmpUEQ(leftValue, rightValue, "cmptmp");
      else 
        return builder.CreateICmpEQ(leftValue, rightValue, "cmptmp");

    else if (op == "!=") 
      if (leftValue->getType()->isFloatTy() && rightValue->getType()->isFloatTy()) 
        return builder.CreateFCmpUNE(leftValue, rightValue, "cmptmp");
      else 
        return builder.CreateICmpNE(leftValue, rightValue, "cmptmp");
    
    else if (op == "<") 
      if (leftValue->getType()->isFloatTy() && rightValue->getType()->isFloatTy()) 
        return builder.CreateFCmpULT(leftValue, rightValue, "cmptmp");
      else 
        return builder.CreateICmpSLT(leftValue, rightValue, "cmptmp");
    
    else if (op == "<=") 
      if (leftValue->getType()->isFloatTy() && rightValue->getType()->isFloatTy()) 
        return builder.CreateFCmpULE(leftValue, rightValue, "cmptmp");
      else 
        return builder.CreateICmpSLE(leftValue, rightValue, "cmptmp");
    
    else if (op == ">") 
      if (leftValue->getType()->isFloatTy() && rightValue->getType()->isFloatTy()) 
        return builder.CreateFCmpUGT(leftValue, rightValue, "cmptmp");
      else 
        return builder.CreateICmpSGT(leftValue, rightValue, "cmptmp");
    
    else if (op == ">=") 
      if (leftValue->getType()->isFloatTy() && rightValue->getType()->isFloatTy()) 
        return builder.CreateFCmpUGE(leftValue, rightValue, "cmptmp");
      else 
        return builder.CreateICmpSGE(leftValue, rightValue, "cmptmp");
    
    else {
      error("Unknown binary operator"); 
      return nullptr;
    }
  }

private:
  LLVM(): context(), module(std::make_unique<llvm::Module>("MyModule", context)), builder(context), scope() {}

  // Removing copy constructor and assignment operator to ensure singleton
  LLVM(const LLVM&) = delete;
  LLVM& operator=(const LLVM&) = delete;
};
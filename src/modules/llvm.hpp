#pragma once

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

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
      return llvm::ConstantFP::get(context, llvm::APFloat(std::stof(statement->getValue())));
    
    else if (Char* statement = dynamic_cast<Char*>(node)) 
      return builder.getInt8(static_cast<int>(statement->getValue()[0]));
    
    else if (Boolean* statement = dynamic_cast<Boolean*>(node)) 
      return builder.getInt1(statement->getValue() == "true");
    
    else if (BinaryOperator* statement = dynamic_cast<BinaryOperator*>(node)) 
      return generateBinaryExpression(statement);

    else if (dynamic_cast<Null*>(node)) 
      return llvm::Constant::getNullValue(getLLVMType("null"));

    else {
      error("Can't convert expression to an valid LLVM value");
      return nullptr;
    }
  }

  llvm::Value* generateBinaryExpression(BinaryOperator* binExpr) {
    llvm::Value* leftValue = getLLVMValue(binExpr->getLeftOperand());
    llvm::Value* rightValue = getLLVMValue(binExpr->getRightOperand());
    
    if (binExpr->getOperator() == "+") 
      return builder.CreateAdd(leftValue, rightValue, "addtmp");
    else if (binExpr->getOperator() == "-") 
      return builder.CreateSub(leftValue, rightValue, "subtmp");
    else if (binExpr->getOperator() == "*") 
      return builder.CreateMul(leftValue, rightValue, "multmp");
    else if (binExpr->getOperator() == "/") 
      return builder.CreateSDiv(leftValue, rightValue, "divtmp");
    else {
      error("Unknown binary operator");
      return nullptr;
    }
  }

private:
  LLVM(): context(), module(std::make_unique<llvm::Module>("MyModule", context)), builder(context) {}

  // Removing copy constructor and assignment operator to ensure singleton
  LLVM(const LLVM&) = delete;
  LLVM& operator=(const LLVM&) = delete;
};
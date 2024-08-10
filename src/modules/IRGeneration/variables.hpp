// #pragma once

// #include <iostream>
// #include <vector>
// #include <memory>

// #include "./llvm.hpp"
// #include "../ast.hpp"
// #include "../error.hpp"

// class VarGen{
// public:
//   VarGen(Variable* node, const bool isLocal): 
//     m_isLocal(isLocal), m_keyword(node->getKeyword()), m_name(node->getName()),
//     m_type(node->getType()), m_value(node->getValue()),
//     llvm(LLVM::getInstance()) { generate(); }

//   void generate() {
//     m_isLocal ? localVariable() : globalVariable();
//   }

// private:
//   bool m_isLocal;
//   string m_keyword;
//   string m_name;
//   string m_type;
//   Expression* m_value;
//   LLVM& llvm;

//   void localVariable(){
//     llvm::Type* type = llvm.getLLVMType(m_type);
//     llvm::AllocaInst* variable = llvm.builder.CreateAlloca(type, nullptr, m_name);

//     if (!(dynamic_cast<Null*>(m_value))){
//       llvm::Value* value = llvm.getLLVMValue(m_value);
//       llvm.builder.CreateStore(llvm::cast<llvm::Constant>(value), variable);
//     }
//     llvm.scope.declare(m_name, variable);
//   }

// void globalVariable(){
//     bool isConstant = (m_keyword == "const");
//     llvm::Constant* value = llvm::cast<llvm::Constant>(llvm.getLLVMValue(m_value));
//     llvm::Type* type = llvm.getLLVMType(m_type);
//     new llvm::GlobalVariable(*llvm.module, type, isConstant, llvm::GlobalValue::ExternalLinkage, value, m_name);
//   }
// };
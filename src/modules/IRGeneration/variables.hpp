#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include "../llvm.hpp"
#include "../ast.hpp"
#include "../error.hpp"

class VarGen{
public:
  VarGen(Variable* node, const bool isLocal): 
    m_isLocal(isLocal), m_keyword(node->getKeyword()), m_name(node->getName()),
    m_type(node->getType()), m_value(node->getValue()),
    llvm(LLVM::getInstance()) { generate(); }

  void generate() {
    m_isLocal ? localVariable() : globalVariable();
  }

private:
  bool m_isLocal;
  string m_keyword;
  string m_name;
  string m_type;
  Expression* m_value;
  LLVM& llvm;

  void localVariable(){
    
  }

void globalVariable(){
    bool isConstant = (m_keyword == "const");
    llvm::Constant* value = llvm::cast<llvm::Constant>(llvm.getLLVMValue(m_value));
    llvm::Type* type = llvm.getLLVMType(m_type);
    new llvm::GlobalVariable(*llvm.module, type, isConstant, llvm::GlobalValue::ExternalLinkage, value, m_name);
}



};
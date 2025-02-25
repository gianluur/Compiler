#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

#include "../includes/ast.h"

using std::string;
using std::unique_ptr;

class Codegen {
public:

  //Constructor
  Codegen(const vector<unique_ptr<ASTNode>>& ast); 

  void generateIR();

  //Getter & Setter
  llvm::LLVMContext& getContext();
  llvm::Module* getModule();
  llvm::IRBuilder<>& getBuilder();

  //Code Generation Methods
  void visit(const AssignmentOperator* statement);
  void visit(const BinaryOperator* statement);
  void visit(const Body* statement);
  void visit(const Cast* statement);
  void visit(const DotOperator* statement);
  void visit(const DoWhile* statement);
  void visit(const Else* statement);
  void visit(const Expression* statement);
  void visit(const For* statement);
  void visit(const Function* statement);
  void visit(const FunctionCall* statement);
  void visit(const Identifier* statement);
  void visit(const If* statement);
  void visit(const ListInitializer* statement);
  void visit(const Literal* statement);
  void visit(const LoopControl* statement);
  void visit(const Operator* statement);
  void visit(const Parameter* statement);
  void visit(const Return* statement);
  void visit(const Struct* statement);
  void visit(const Type* statement);
  void visit(const UnaryOperator* statement);
  void visit(const Variable* statement);
  void visit(const While* statement);
  

private:
  const vector<unique_ptr<ASTNode>>& ast;
  llvm::LLVMContext context;
  unique_ptr<llvm::Module> module;
  llvm::IRBuilder<> builder;
};
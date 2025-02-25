#include "codegen.h"

Codegen::Codegen(const vector<unique_ptr<ASTNode>>& ast):
  ast(std::move(ast)), module(std::make_unique<llvm::Module>("module", context)), builder(context) {}

void Codegen::generateIR(){
  for(const unique_ptr<ASTNode>& node: ast)
    node->accept(this);
}

llvm::LLVMContext& Codegen::getContext(){
  return context;
}

llvm::Module* Codegen::getModule() {
  return module.get();
}

llvm::IRBuilder<>& Codegen::getBuilder(){
  return builder;
}

void Codegen::visit(const AssignmentOperator* statement) { statement->print(); }
void Codegen::visit(const BinaryOperator* statement) { statement->print(); }
void Codegen::visit(const Body* statement) { statement->print(); }
void Codegen::visit(const Cast* statement) { statement->print(); }
void Codegen::visit(const DotOperator* statement) { statement->print(); }
void Codegen::visit(const DoWhile* statement) { statement->print(); }
void Codegen::visit(const Else* statement) { statement->print(); }
void Codegen::visit(const Expression* statement) { statement->print(); }
void Codegen::visit(const For* statement) { statement->print(); }
void Codegen::visit(const Function* statement) { statement->print(); }
void Codegen::visit(const FunctionCall* statement) { statement->print(); }
void Codegen::visit(const Identifier* statement) { statement->print(); }
void Codegen::visit(const If* statement) { statement->print(); }
void Codegen::visit(const ListInitializer* statement) { statement->print(); }
void Codegen::visit(const Literal* statement) { statement->print(); }
void Codegen::visit(const LoopControl* statement) { statement->print(); }
void Codegen::visit(const Operator* statement) { statement->print(); }
void Codegen::visit(const Parameter* statement) { statement->print(); }
void Codegen::visit(const Return* statement) { statement->print(); }
void Codegen::visit(const Struct* statement) { statement->print(); }
void Codegen::visit(const Type* statement) { statement->print(); }
void Codegen::visit(const UnaryOperator* statement) { statement->print(); }
void Codegen::visit(const Variable* statement) { statement->print(); }
void Codegen::visit(const While* statement) { statement->print(); }

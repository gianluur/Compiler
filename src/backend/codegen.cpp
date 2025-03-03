#include "codegen.h"

Codegen::Codegen(const vector<unique_ptr<ASTNode>>& ast):
  ast(std::move(ast)), module(std::make_unique<llvm::Module>("module", context)), builder(context), scope() { generateIR(); }

void Codegen::generateIR(){
  for(const unique_ptr<ASTNode>& node: ast)
    node->accept(this);
  module->print(llvm::outs(), nullptr);
  executeIR();
}

void Codegen::executeIR(){
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  string error;
  llvm::ExecutionEngine* executionEngine = 
    llvm::EngineBuilder(std::move(module))
      .setErrorStr(&error)
      .setMCJITMemoryManager(std::make_unique<llvm::SectionMemoryManager>())
      .create();

  if (!executionEngine) {
    llvm::errs() << "Failed to create ExecutionEngine: " << error << "\n";
    return;
  }

  executionEngine->finalizeObject();  // Ensure IR is compiled

  // Get the function pointer to "main" (or another function)
  void (*mainFunction)() = (void (*)())executionEngine->getFunctionAddress("main");
  if (!mainFunction) {
    llvm::errs() << "Function 'main' not found in generated IR\n";
    return;
  }

  // Run the generated function
  mainFunction();
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

llvm::Type* Codegen::getLLVMType(const ASTNodeType type, const string& str_type = ""){
  switch (type) {
    case ASTNodeType::BOOL:
      return llvm::Type::getInt1Ty(context);

    case ASTNodeType::CHAR:
    case ASTNodeType::INT8:
      return llvm::Type::getInt8Ty(context);

    case ASTNodeType::INT16:
      return llvm::Type::getInt16Ty(context);

    case ASTNodeType::INT32:
      return llvm::Type::getInt32Ty(context);

    case ASTNodeType::INT64:
      return llvm::Type::getInt64Ty(context);
    
    case ASTNodeType::INT:
      return llvm::Type::getInt32Ty(context);

    case ASTNodeType::FLOAT32:
      return llvm::Type::getFloatTy(context);

    case ASTNodeType::FLOAT64:
      return llvm::Type::getDoubleTy(context);

    case ASTNodeType::FLOAT:
      return llvm::Type::getDoubleTy(context);

    case ASTNodeType::NOTHING:
      return llvm::Type::getVoidTy(context);
    
    case ASTNodeType::IDENTIFIER:
      return llvm::StructType::getTypeByName(context, str_type);

    default:
      error("Couldn't generate type to a valid LLVM Type");
  }
}

llvm::Value* Codegen::getLLVMValue(const ASTNode* value){
  switch(value->getNodeType()) {
    case ASTNodeType::LITERAL_INTEGER:
      return builder.getInt32(std::stoi(dynamic_cast<const Literal*>(value)->toString()));

    case ASTNodeType::LITERAL_FLOAT:
      return llvm::ConstantFP::get(llvm::Type::getFloatTy(context), std::stod(dynamic_cast<const Literal*>(value)->toString()));

    case ASTNodeType::LITERAL_CHARACTER:
      return builder.getInt8(static_cast<int>(dynamic_cast<const Literal*>(value)->toString()[0]));

    case ASTNodeType::LITERAL_BOOLEAN:
      return builder.getInt1(dynamic_cast<const Literal*>(value)->toString()[0] == 't'); // if it's t is' true other wise it's false (first char aren't equals)

    case ASTNodeType::BINARY_OPERATOR:
      return generateBinaryOperator(dynamic_cast<const BinaryOperator*>(value));

    case ASTNodeType::UNARY_OPERATOR:
      return generateUnaryOperator(dynamic_cast<const UnaryOperator*>(value));

    case ASTNodeType::CAST:
      return generateCast(dynamic_cast<const Cast*>(value));

    default:
      error("Couldn't convert expression to a valid LLVM Value");
  }
}

llvm::Value* Codegen::generateBinaryOperator(const BinaryOperator* statement) {
  // #[DEBUG]
  if (statement == nullptr) {
    error("statement is null");
  }

  llvm::Value* left = getLLVMValue(statement->getLeft());
  llvm::Value* right = getLLVMValue(statement->getRight());
  const TokenType op = statement->getOperator();
  
  switch (op) {
    case TokenType::EQUALS:
      if (left->getType()->isFloatTy() && right->getType()->isFloatTy()) 
        return builder.CreateFCmpUEQ(left, right, "cmptmp");
      else 
        return builder.CreateICmpEQ(left, right, "cmptmp");
    
    case TokenType::NOT_EQUAL:
      if (left->getType()->isFloatTy() && right->getType()->isFloatTy()) 
        return builder.CreateFCmpUNE(left, right, "cmptmp");
      else 
        return builder.CreateICmpNE(left, right, "cmptmp");
    
    case TokenType::GREATER:
      if (left->getType()->isFloatTy() && right->getType()->isFloatTy()) 
        return builder.CreateFCmpUGT(left, right, "cmptmp");
      else 
        return builder.CreateICmpSGT(left, right, "cmptmp");
    
    case TokenType::LESS:
      if (left->getType()->isFloatTy() && right->getType()->isFloatTy()) 
        return builder.CreateFCmpULT(left, right, "cmptmp");
      else 
        return builder.CreateICmpSLT(left, right, "cmptmp");
    
    case TokenType::GREATER_EQUAL:
      if (left->getType()->isFloatTy() && right->getType()->isFloatTy()) 
        return builder.CreateFCmpUGE(left, right, "cmptmp");
      else 
        return builder.CreateICmpSLE(left, right, "cmptmp");
    
    case TokenType::LESS_EQUAL:
      if (left->getType()->isFloatTy() && right->getType()->isFloatTy()) 
        return builder.CreateFCmpULE(left, right, "cmptmp");
      else 
        return builder.CreateICmpSLE(left, right, "cmptmp");
    
    case TokenType::ADDITION:
      if (left->getType()->isFloatTy() && right->getType()->isFloatTy()) 
        return builder.CreateFAdd(left, right, "faddtmp");
      else 
        return builder.CreateAdd(left, right, "addtmp");
    
    case TokenType::SUBTRACTION:
      if (left->getType()->isFloatTy() && right->getType()->isFloatTy()) 
        return builder.CreateFSub(left, right, "faddtmp");
      else 
        return builder.CreateSub(left, right, "addtmp");
    
    case TokenType::STAR:
      if (left->getType()->isFloatTy() && right->getType()->isFloatTy()) 
        return builder.CreateFMul(left, right, "faddtmp");
      else 
        return builder.CreateMul(left, right, "addtmp");
    
    case TokenType::DIVISION:
      if (left->getType()->isFloatTy() && right->getType()->isFloatTy()) 
        return builder.CreateFDiv(left, right, "faddtmp");
      else 
        return builder.CreateSDiv(left, right, "addtmp");
    
    case TokenType::MODULUS:
      if (left->getType()->isFloatTy() && right->getType()->isFloatTy()) 
        return builder.CreateFRem(left, right, "faddtmp");
      else 
        return builder.CreateSRem(left, right, "addtmp");
    
    default:
      error("Invalid operator in binary operation");   
  }
}

llvm::Value* Codegen::generateUnaryOperator(const UnaryOperator* statement){
  // #[DEBUG]
  if (statement == nullptr) {
    error("statement is null");
  }

  llvm::Value* right = getLLVMValue(statement->getRight());
  const TokenType op = statement->getOperator();

  switch (op) {
    case TokenType::NOT:
      return builder.CreateICmpEQ(right, llvm::ConstantInt::get(right->getType(), 0));

    case TokenType::AMPERSAND:
    case TokenType::CARET:
      error("Implement pointers in unary operator"); // TODO Add pointer dereference
    
    default:
      error("Invalid operator in unary operator");
  }
}

llvm::Value* Codegen::generateCast(const Cast* statement){
  // #[DEBUG]
  if (statement == nullptr) {
    error("statement is null");
  }

  llvm::Type* castType = getLLVMType(statement->getType());
  llvm::Value* value = getLLVMValue(statement->getExpression());
  llvm::Type* valueType = value->getType();  

  if (valueType->isIntegerTy() && castType->isFloatTy())
    return builder.CreateSIToFP(value, castType, "intToFloat");

  else if (valueType->isFloatTy() && castType == llvm::Type::getInt32Ty(context))
    return builder.CreateFPToSI(value, castType, "floatToInt");

  else if (valueType == llvm::Type::getInt8Ty(context) && castType == llvm::Type::getInt32Ty(context))
    return builder.CreateZExt(value, castType, "charToInt");

  else if (valueType == llvm::Type::getInt32Ty(context) && castType == llvm::Type::getInt8Ty(context))
    return builder.CreateTrunc(value, castType, "intToChar");

  else if (valueType->isIntegerTy() && castType == llvm::Type::getInt1Ty(context))
    return builder.CreateICmpNE(value, llvm::ConstantInt::get(valueType, 0), "intToBool");
  
  else if (valueType == llvm::Type::getInt1Ty(context) && castType->isIntegerTy())
    return builder.CreateZExt(value, castType, "boolToInt");
  
  else if (valueType->isFloatTy() && castType == llvm::Type::getInt1Ty(context))
    return builder.CreateFCmpONE(value, llvm::ConstantFP::get(valueType, 0.0), "floatToBool");
  
  else {
    error("Couldn't generate a cast expression");
  }
}

vector<llvm::Type*> Codegen::getParametersType(const vector<Parameter*>& parameters){
  vector<llvm::Type*> types;

  for (const Parameter* parameter : parameters) {
    llvm::Type* llvmType = getLLVMType(parameter->getType());
    types.emplace_back(llvmType);
  }
  return types;
}

void Codegen::visit(const AssignmentOperator* statement) { statement->print(); }
void Codegen::visit(const BinaryOperator* statement) { statement->print(); }
void Codegen::visit(const Body* statement) { statement->print(); }
void Codegen::visit(const Cast* statement) { statement->print(); }
void Codegen::visit(const DotOperator* statement) { statement->print(); }
void Codegen::visit(const DoWhile* statement) { statement->print(); }
void Codegen::visit(const Else* statement) { statement->print(); }
void Codegen::visit(const Expression* statement) {
  statement->print(); 
}
void Codegen::visit(const For* statement) { statement->print(); }

void Codegen::visit(const Function* statement) {
  const ASTNodeType AST_ReturnType = statement->getType();
  const Identifier* AST_Identifier = statement->getIdentifier();
  const vector<Parameter*> AST_Parameters = statement->getParameter();
  const vector<ASTNode*> AST_Body = statement->getBody();

  llvm::Type* IR_ReturnType = getLLVMType(AST_ReturnType);
  vector<llvm::Type*> IR_Parameters = getParametersType(AST_Parameters);
  llvm::FunctionType* IR_type = llvm::FunctionType::get(IR_ReturnType, IR_Parameters, false);

  llvm::Function* function = llvm::Function::Create(IR_type, llvm::Function::ExternalLinkage, AST_Identifier->toString(), module.get());
  llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", function);
  builder.SetInsertPoint(BB);

  scope.enterScope();

  size_t index = 0;
  for (llvm::Argument& argument : function->args()) {
    const Parameter* parameter = AST_Parameters.at(index);
    argument.setName(parameter->getIdentifier());
    
    llvm::AllocaInst* variable = builder.CreateAlloca(argument.getType(), nullptr, parameter->getIdentifier() + "_addr");
    builder.CreateStore(&argument, variable);
    scope.declareVariable(parameter->getIdentifier(), variable);
    
    index++;
  }

  for(const ASTNode* node: AST_Body){
    node->accept(this);
  }

  scope.exitScope();

  scope.declareFunction(AST_Identifier->toString(), function);
  llvm::verifyFunction(*function);
}

void Codegen::visit(const FunctionCall* statement) { statement->print(); }
void Codegen::visit(const Identifier* statement) { statement->print(); }
void Codegen::visit(const If* statement) { statement->print(); }
void Codegen::visit(const ListInitializer* statement) { statement->print(); }
void Codegen::visit(const Literal* statement) { statement->print(); }
void Codegen::visit(const LoopControl* statement) { statement->print(); }
void Codegen::visit(const Operator* statement) { statement->print(); }
void Codegen::visit(const Parameter* statement) { statement->print(); }

void Codegen::visit(const Return* statement) { 
  const ASTNode* AST_Value = statement->getValue()->getASTNode();

  llvm::Value* IR_Value = getLLVMValue(AST_Value);
  builder.CreateRet(IR_Value);
}

void Codegen::visit(const Struct* statement) { statement->print(); }
void Codegen::visit(const Type* statement) { statement->print(); }
void Codegen::visit(const UnaryOperator* statement) { statement->print(); }

void Codegen::visit(const Variable* statement) { 
  statement->print(); 
}

void Codegen::visit(const While* statement) { statement->print(); }

#include "expression.h"
#include "ASTNode.h"

#include "../../backend/codegen.h"

Expression::Expression(unique_ptr<ASTNode> start, const bool isCondition): 
  ASTNode(start->getNodeType()), m_start(std::move(start)), m_isCondition(isCondition) {
    if (m_isCondition)
      Expression::analyzeCondition(m_start.get());
    else 
      m_type = Expression::analyzeExpression(m_start.get());
  }
Expression::Expression(): 
  ASTNode(ASTNodeType::NOTHING), m_start(make_unique<Literal>()) {}

void Expression::accept(Codegen* generator) const {
  generator->visit(this);
}

void Expression::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Expression {\n";
  m_start->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

ASTNode* Expression::getASTNode() const {
  return m_start.get();
}

ASTNodeType Expression::getType() const {
  return m_type;
}

ASTNodeType Expression::analyzeExpression(const ASTNode* expression) {
  const ASTNodeType type = expression->getNodeType();

  switch (type) {
    case ASTNodeType::LITERAL_INTEGER:
      return ASTNodeType::INT;

    case ASTNodeType::LITERAL_FLOAT:
      return ASTNodeType::FLOAT;

    case ASTNodeType::LITERAL_CHARACTER:
      return ASTNodeType::CHAR;
  
    case ASTNodeType::LITERAL_BOOLEAN:
      return ASTNodeType::BOOL;

    case ASTNodeType::NOTHING:
      return ASTNodeType::NOTHING;

    case ASTNodeType::IDENTIFIER: 
    if (const Identifier* identifier = dynamic_cast<const Identifier*>(expression))
      return identifier->getIdentifierType(identifier);      

    case ASTNodeType::CAST:
      if (const Cast* cast = dynamic_cast<const Cast*>(expression))
        return cast->analyzeCast();

    case ASTNodeType::BINARY_OPERATOR:
      if (const BinaryOperator* binaryOperator = dynamic_cast<const BinaryOperator*>(expression))
        return binaryOperator->analyzeBinaryOperator(binaryOperator);

    case ASTNodeType::UNARY_OPERATOR:
      if (const UnaryOperator* unaryOperator = dynamic_cast<const UnaryOperator*>(expression))
        return unaryOperator->analyzeUnaryOperator(unaryOperator);

    case ASTNodeType::FUNCTION_CALL:
      if (const FunctionCall* functionCall = dynamic_cast<const FunctionCall*>(expression))
        return functionCall->analyzeFunctionCall(functionCall);
      
    case ASTNodeType::DOT_OPERATOR:
      if (const DotOperator* dotOperator = dynamic_cast<const DotOperator*>(expression))
        return dotOperator->getMemberType(dotOperator);

    default:
      error("Unexpected error when analyzing expression type: " + std::to_string(static_cast<int>(type)));
      break;
  }
}

void Expression::analyzeCondition(const ASTNode* condition) {
  if (analyzeExpression(condition) != ASTNodeType::BOOL)
    error("Conditions must always evaluate to boolean");
}
#include "expression.h"
#include "ASTNode.h"

Expression::Expression(unique_ptr<ASTNode> start): 
  ASTNode(start->getNodeType()), m_start(std::move(start)) {
    Expression::analyzeExpression(m_start.get());
  }
Expression::Expression(): 
  ASTNode(ASTNodeType::NULL), m_start(make_unique<Literal>()) {}

void Expression::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Expression {\n";
  m_start->print(indentation_level + 2);
  cout << setw(indentation_level) << " " << "}\n";
}

ASTNode* Expression::getExpression() const {
  return m_start.get();
}

ASTNodeType Expression::getExpressionType() const {
  return getNodeType();
}

ASTNodeType Expression::analyzeExpression(const ASTNode* expression) {
  const ASTNodeType type = expression->getNodeType();
  switch (type) {
    case ASTNodeType::LITERAL_INTEGER:
    case ASTNodeType::LITERAL_FLOAT:
    case ASTNodeType::LITERAL_CHARACTER:
    case ASTNodeType::LITERAL_BOOLEAN:
      return type;
    
    case ASTNodeType::IDENTIFIER: 
    if (const Identifier* identifier = dynamic_cast<const Identifier*>(expression))
      return identifier->getIdentifierType(identifier);      

    case ASTNodeType::CAST:
      if (const Cast* cast = dynamic_cast<const Cast*>(expression))
        return cast->analyzeCast(cast);

    case ASTNodeType::BINARY_OPERATOR:
      if (const BinaryOperator* binaryOperator = dynamic_cast<const BinaryOperator*>(expression))
        return binaryOperator->analyzeBinaryOperator(binaryOperator);

    case ASTNodeType::UNARY_OPERATOR:
      if (const UnaryOperator* unaryOperator = dynamic_cast<const UnaryOperator*>(expression))
        return unaryOperator->analyzeUnaryOperator(unaryOperator);

    case ASTNodeType::FUNCTION_CALL:
      if (const FunctionCall* functionCall = dynamic_cast<const FunctionCall*>(expression))
        return functionCall->getFunctionCallType(functionCall);
      
    // case ASTNodeType::DOT_OPERATOR:
    //   if (const DotOperator* dotOperator = dynamic_cast<const DotOperator*>(expression))
    //     return dotOperator->analyzeDotOperator(dotOperator);

    default:
      error("Unexpected error when analyzing expression type");
      break;
  }
}
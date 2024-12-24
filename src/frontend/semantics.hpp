#pragma once

#include <climits>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <variant>

#include "../includes/error.hpp"
#include "../includes/ast.h"

using std::cout;
using std::vector, std::string, std::unordered_map; 
using std::unique_ptr, std::make_unique;
using std::variant;

// namespace Semantics {
//   static ASTNodeType analyzeExpression(const Expression* expression);

//   static ASTNodeType analyzeBinaryOperator(const BinaryOperator* binaryOperator) {
//     const ASTNodeType leftOperand = binaryOperator->getLeft();
//     const ASTNodeType rightOperand = binaryOperator->getRight();
    
//     if (leftOperand != rightOperand) 
//       error("In expressions the left and right operand in a binary operator must have the same type");
//     return leftOperand; 
//   }

//   static ASTNodeType analyzeUnaryOperator(const UnaryOperator* unaryOperator) {

//   }

//   static ASTNodeType analyzeFunctionCall(const FunctionCall* functionCall) {

//   }

//   static ASTNodeType analyzeDotOperator(const DotOperator* dotOperator) {

//   }

//   static ASTNodeType analyzeIdentifier(const Identifier* identifier) {

//   }

//   static ASTNodeType analyzeCast(const Cast* cast){

//   }

//   static ASTNodeType analyzeExpression(const Expression* expression){
//     const ASTNodeType type = expression->getExpressionType();
//     switch (type) {
//       case ASTNodeType::LITERAL_INTEGER:
//       case ASTNodeType::LITERAL_FLOAT:
//       case ASTNodeType::LITERAL_CHARACTER:
//       case ASTNodeType::LITERAL_BOOLEAN:
//         return type;
      
//       case ASTNodeType::IDENTIFIER: 
//       if (const Identifier* identifier = dynamic_cast<const Identifier*>(expression))
//         return analyzeIdentifier(identifier);      

//       // case ASTNodeType::CAST:
//       //   if (const Cast* cast = dynamic_cast<const Cast*>(expression))
//       //     return analyzeCast(cast);

//       case ASTNodeType::BINARY_OPERATOR:
//         if (const BinaryOperator* binaryOperator = dynamic_cast<const BinaryOperator*>(expression))
//           return analyzeBinaryOperator(binaryOperator);

//       case ASTNodeType::UNARY_OPERATOR:
//         if (const UnaryOperator* unaryOperator = dynamic_cast<const UnaryOperator*>(expression))
//           return analyzeUnaryOperator(unaryOperator);

//       case ASTNodeType::FUNCTION_CALL:
//         if (const FunctionCall* functionCall = dynamic_cast<const FunctionCall*>(expression))
//           return analyzeFunctionCall(functionCall);
        
//       case ASTNodeType::DOT_OPERATOR:
//         if (const DotOperator* dotOperator = dynamic_cast<const DotOperator*>(expression))
//           return analyzeDotOperator(dotOperator);

//       default:
//         error("Unexpected error when analyzing expression type");
//         break;
//     }
//   }
// };
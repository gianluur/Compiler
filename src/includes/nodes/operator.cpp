#include "operator.h"
#include "ASTNode.h"

#include "../../backend/codegen.h"

Operator::Operator(const Token& token): 
  ASTNode(ASTNodeType::OPERATOR), op(token.type), m_str(token.lexemes) {}

void Operator::accept(Codegen* generator) const {
  generator->visit(this);
}

void Operator::print(int indentation_level) const {
  cout << setw(indentation_level) << " " << "Operator: " << m_str << '\n';
}

enum TokenType Operator::getOperator() const {
  return op;
}

string Operator::toString() const {
  return m_str;
}

bool Operator::isComparisonOperator() const {
  return op >= TokenType::OPERATOR_COMPARISON_BEGIN && op <= TokenType::OPERATOR_COMPARISON_END;
}
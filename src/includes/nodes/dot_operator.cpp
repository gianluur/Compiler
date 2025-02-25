#include "dot_operator.h"
#include "ASTNode.h"
#include "variable.h"
#include "struct.h"

#include "../../backend/codegen.h"

DotOperator::DotOperator(unique_ptr<Identifier> identifier, unique_ptr<AssignmentOperator> assigment):
  ASTNode(ASTNodeType::DOT_OPERATOR), m_identifier(std::move(identifier)), m_assigment(std::move(assigment)), m_member(nullptr) {}

DotOperator::DotOperator(unique_ptr<Identifier> identifier, unique_ptr<Identifier> member):
  ASTNode(ASTNodeType::DOT_OPERATOR), m_identifier(std::move(identifier)), m_assigment(nullptr), m_member(std::move(member)) {}

void DotOperator::accept(Codegen* generator) const {
  generator->visit(this);
}

void DotOperator::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "DotOperator: {\n";
  m_identifier->print(indentation_level + 2);
  if (m_assigment){
    cout << setw(indentation_level + 2) << " " << "Member: " << m_assigment->getIdentifier()->toString() << '\n';
    cout << setw(indentation_level + 2) << " " << "Operator: " << m_assigment->getOperatorToString() << '\n';
    m_assigment->getExpression()->print(indentation_level + 2);
  }
  else 
    cout << setw(indentation_level + 2) << " " << "Member: " << m_member->toString() << '\n';

  cout << setw(indentation_level) << " " << "}\n";  
}

ASTNodeType DotOperator::getMemberType(const DotOperator* dotOperator) const {
  const Struct* structure = Struct::getStructure(dotOperator->m_identifier->toString());
  const size_t index = structure->getMemberIndex(dotOperator->m_member->toString());
  return structure->getMember(index)->getType();
}
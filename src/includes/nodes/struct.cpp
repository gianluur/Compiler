#include "struct.h"
#include "ASTNode.h"
#include "variable.h"

Struct::Struct(unique_ptr<Identifier> identifier, vector<unique_ptr<Variable>> members):
  ASTNode(ASTNodeType::STRUCTURE), m_identifier(std::move(identifier)), m_members(std::move(members)) {}

void Struct::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "Struct {\n";
  m_identifier->print(indentation_level + 2);
  for(const unique_ptr<Variable>& member: m_members){
    member->print(indentation_level + 2);
  }
  cout << setw(indentation_level) << " " << "}\n";
}

vector<Variable*> Struct::getMembers() const {
  vector<Variable*> members;
  for(const unique_ptr<Variable>& member: m_members){
    members.push_back(member.get());
  }
  return members;
}
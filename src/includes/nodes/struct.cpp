#include "struct.h"
#include "ASTNode.h"
#include "identifier.h"
#include "variable.h"

Struct::Struct(unique_ptr<Identifier> identifier, vector<unique_ptr<Variable>> members):
  ASTNode(ASTNodeType::STRUCTURE), m_identifier(std::move(identifier)), m_members(std::move(members)) {
    analyzeStruct();
  }

void Struct::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "Struct {\n";
  m_identifier->print(indentation_level + 2);
  cout << setw(indentation_level + 2) << " " << "Body: {\n";
  for(const unique_ptr<Variable>& member: m_members){
    member->print(indentation_level + 4);
  }
  cout << setw(indentation_level + 2) << " " << "}\n";
  cout << setw(indentation_level) << " " << "}\n";
}

vector<Variable*> Struct::getMembers() const {
  vector<Variable*> members;
  for(const unique_ptr<Variable>& member: m_members){
    members.push_back(member.get());
  }
  return members;
}

size_t Struct::getMembersSize() const {
  return m_members.size();
}

size_t Struct::getMemberIndex(const string& identifier) const {
  for (size_t index = 0; index < m_members.size(); index++) {
    if (m_members[index]->getIdentifier() == identifier)
      return index;
  }
  error("Couldn't find the member: " + identifier);
}

const Variable* Struct::getMember(const size_t index) const {
  return m_members[index].get();
}

const Struct* Struct::getStructure(const string& identifer) {
  const Symbol& variable = Scope::getInstance()->find(identifer);
  const Symbol& structSymbol = Scope::getInstance()->find(std::get<const Variable*>(variable.symbol)->getTypeToString());
  return std::get<const Struct*>(structSymbol.symbol);
}

void Struct::analyzeStruct() const {
  Scope::getInstance()->declare(m_identifier->toString(), Symbol(this));
} 
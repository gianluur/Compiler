#include "ASTNode.h"
#include "expression.h"
#include "identifier.h"
#include "variable.h"

class Struct: public ASTNode {
public:
  Struct(unique_ptr<Identifier> identifier, vector<unique_ptr<Variable>> members);
  void print(int indentation_level = 0) const override;
  vector<Variable*> getMembers() const;
  size_t getMembersSize() const;
  size_t getMemberIndex(const string& identifier) const; 
  const Variable* getMember(const size_t index) const;
  static const Struct* getStructure(const string& identifer);
  void analyzeStruct() const;
  
private:
  unique_ptr<Identifier> m_identifier;
  vector<unique_ptr<Variable>> m_members;
};

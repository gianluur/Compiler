#include "ASTNode.h"
#include "expression.h"
#include "identifier.h"
#include "variable.h"

class Struct: public ASTNode {
public:
  Struct(unique_ptr<Identifier> identifier, vector<unique_ptr<Variable>> members);
  void print(int indentation_level = 0) const override;
  vector<Variable*> getMembers() const;
  void analyzeStruct() const;
  
private:
  unique_ptr<Identifier> m_identifier;
  vector<unique_ptr<Variable>> m_members;
};

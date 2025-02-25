#include "else.h"

#include "../../backend/codegen.h"

Else::Else(unique_ptr<If> ifstatement): 
  ASTNode(ASTNodeType::ELSE), m_ifstatement(std::move(ifstatement)) {} //idk if this should be 'if' or 'else'

Else::Else(unique_ptr<Body> body): 
  ASTNode(ASTNodeType::ELSE), m_body(std::move(body)) {}

void Else::accept(Codegen* generator) const {
  generator->visit(this);
}

void Else::print(int indentation_level) const {
  cout << '\n' << setw(indentation_level) << " " << "Else Statement {\n";
  if (!m_ifstatement)
    m_body->print(indentation_level + 2);
  else
    m_ifstatement->print(indentation_level + 2);        
  cout << setw(indentation_level) << " " << "} " << endl;
}

vector<ASTNode*> Else::getBody() const {
  return m_body->getStatements();
}
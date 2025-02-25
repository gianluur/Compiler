#pragma once

#include "ASTNode.h"
#include "if.h"
#include "body.h"

class If;
class Else: public ASTNode {
public:
  Else(unique_ptr<If> ifstatement);
  Else(unique_ptr<Body> body);

  void accept(Codegen* generator) const override;
  void print(int indentation_level = 0) const override;

  vector<ASTNode*> getBody() const;

private:
  unique_ptr<If> m_ifstatement;
  unique_ptr<Body> m_body;
};
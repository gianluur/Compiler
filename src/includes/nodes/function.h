#pragma once

#include "type.h"
#include "identifier.h"
#include "parameter.h"
#include "body.h"

class Function: public ASTNode {
public:
  Function(unique_ptr<Type> type, unique_ptr<Identifier> identifier, vector<unique_ptr<Parameter>> parameters, unique_ptr<Body> body);
  void print(int indentation_level = 0) const override;
  ASTNodeType getType() const;
  string getIdentifier() const;
  vector<Parameter*> getParameter() const;

private:
  unique_ptr<Type> m_type;
  unique_ptr<Identifier> m_identifier;
  vector<unique_ptr<Parameter>> m_parameters;
  unique_ptr<Body> m_body;
};
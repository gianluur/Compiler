#pragma once

#include "ASTNode.h"
#include "identifier.h"
#include "type.h"

class Parameter : public ASTNode {
public:
  Parameter(unique_ptr<Type> type, unique_ptr<Identifier> name);
  void print(int indentation_level = 0) const override;
  ASTNodeType getType() const;
  string getIdentifier() const;

private:
  unique_ptr<Type> m_type;
  unique_ptr<Identifier> m_identifier;
};
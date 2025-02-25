#pragma once
#include "ASTNode.h"

class Type: public ASTNode {
public:
  Type(const Token& token, const bool isPointer);

  void accept(Codegen* generator) const override;
  void print(int indentation_level = 0) const override;

  ASTNodeType getType() const;
  bool isInteger() const;
  bool isFloat() const;
  bool isBool() const;
  bool isChar() const;
  bool isNull() const;
  bool isPointer() const;
  bool isStruct() const; 
  string toString() const;
  static bool AreEquals(const ASTNodeType type1, const ASTNodeType type2);
  ASTNodeType TokenTypeToASTNodeType(const enum TokenType type) const;

private:
  const enum TokenType m_type;
  const string m_str;
  const bool m_isPointer;
};
#include "ASTNode.h"

class LoopControl: public ASTNode {
public:
  LoopControl(const Token& token, const TokenType scope);
  void print(int indentation_level = 0) const override;
  string getKeyword() const;
  TokenType getScope() const;

private:
  const string m_str;
  const TokenType m_scope;
};
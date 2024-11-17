#include "ASTNode.h"

class LoopControl: public ASTNode {
public:
  LoopControl(const Token& token);
  void print(int indentation_level = 0) const override;
  string getKeyword() const;

private:
  const string m_str;
};
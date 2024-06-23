#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include "astnode.hpp"

using std::cout, std::endl;
using std::vector, std::unique_ptr;


class ASTNode {
public:
  ASTNodeType m_type;

  ASTNode(ASTNodeType type): m_type(type) {}

  virtual ~ASTNode() = default;
};

using ASTNodePtr = unique_ptr<ASTNode>;

class Program : public ASTNode {
public:
  vector<ASTNodePtr> statements;
  Program(): ASTNode(ASTNodeType::PROGRAM) {}

  void print(){
    cout << "----- Program start -----" << endl;
    cout << "----- Program end -----" << endl;
  }

};

class Number: public ASTNode {
public:
  Number(double value): m_value(value), ASTNode(ASTNodeType::NUMBER) {}

private:
  double m_value;
};

class BinaryOperator: public ASTNode {
public:
  BinaryOperator(char op, unique_ptr<ASTNode> left, unique_ptr<ASTNode> right): 
    m_op(op), m_left(std::move(left)), m_right(std::move(right)), ASTNode(ASTNodeType::BINARY_OPERATOR) {}

private:
  char m_op;
  unique_ptr<ASTNode> m_left, m_right;

};

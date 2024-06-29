#include <iostream>
#include <iomanip>
#include <memory>

using std::cout, std::endl, std::setw;
using std::unique_ptr, std::make_unique;

enum class ASTNodeType {
  INVALID,
  NUMBER,
  BINARY_OPERATOR,
};

class Number {
public:
  virtual ~Number() = default;
  virtual void print() const = 0;
};

class Integer: public Number {
public:
  Integer(int value) : m_value(value) { print(); }

  void print() const override {
    cout << "NodeType: Integer " << "| Value: " << m_value << endl;;
  }

private:
  int m_value;
};

class Float: public Number {
public:
  Float(double value) : m_value(value) { print(); }

  void print() const override {
    cout << "NodeType: Float " << "| Value: " << m_value << endl;;
  }

private:
  double m_value;
};

class BinaryOperator {
public:

  BinaryOperator(char op, unique_ptr<Number> left, unique_ptr<Number> right):
    m_operator(op), m_left(move(left)), m_right(move(right)) { print(); }

  void print() {
    cout << "NodeType: Binary Operator" << endl;
    cout << "Operator: " << m_operator << endl;
    cout << "Left: " << endl << setw(2) << "";
    m_left.get()->print();
    cout << "Right: " << endl << setw(2) << "";
    m_right.get()->print();
  }
private:
  char m_operator;
  unique_ptr<Number> m_left;
  unique_ptr<Number> m_right;
};
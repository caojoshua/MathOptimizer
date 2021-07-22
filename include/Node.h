#ifndef NODE_H
#define NODE_H

#include <list>
#include <sstream>
#include <string>

class OperatorNode;

class Node {
public:
  Node() : parent(nullptr){};
  virtual ~Node(){};
  OperatorNode *getParent();
  void setParent(OperatorNode *parent);
  virtual std::string codeGen() = 0;
  virtual std::string toString() = 0;

private:
  OperatorNode *parent;
};

class OperatorNode : public Node {
public:
  enum Op { Add, Sub, Mul, Div };

  enum Precedence { SumPrecedence = 0, ProductPrecedence = 1 };

  struct Parameter {
    Op op;
    Node *node;
  };

  OperatorNode(Precedence precedence, Node *node);
  OperatorNode(Op op, Node *left, Node *right);

  std::string codeGen();
  std::string toString();

  Precedence getPrecedence();
  std::list<Parameter> &getParameters();
  bool appendParameter(Parameter parameter);
  void appendParameter(Node *node);
  bool appendParameter(Op op, Node *node);
  void prependParameter(Node *node);

  static unsigned getOpPrecedence(Op op);
  static Op getOppositeOp(Op op);
  static std::string opToStr(Op op);
  static void unknownOperatorError(Op op);

private:
  Precedence precedence;
  std::list<Parameter> parameters;

  Op getDefaultOp();
  std::string opToStr();
  std::string childNodeCodeGen(Node *child);
};

class IdentifierNode : public Node {
public:
  IdentifierNode(std::string identifier) : Node(), identifier(identifier) {}
  std::string codeGen();
  std::string toString();
  std::string getIdentifier();

private:
  std::string identifier;
};

class NumberNode : public Node {
public:
  NumberNode(float number) : Node(), number(number) {}
  std::string codeGen();
  std::string toString();
  float getNumber();

private:
  static std::ostringstream ss;
  float number;
};

#endif

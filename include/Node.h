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
  virtual bool operator==(Node *other);
  virtual bool operator!=(Node *other);
  virtual Node *clone() = 0;
  virtual std::string codeGen() = 0;
  virtual std::string toString() = 0;
  OperatorNode *getParent();
  void setParent(OperatorNode *parent);

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

  OperatorNode(Precedence precedence) : precedence(precedence){};
  OperatorNode(Precedence precedence, Node *node);
  ~OperatorNode();

  virtual bool operator==(Node *other);
  virtual Node *clone();
  std::string codeGen();
  std::string toString();

  Precedence getPrecedence();
  std::list<Parameter> &getParameters();
  bool appendParameter(Parameter parameter);
  void appendParameter(Node *node);
  bool appendParameter(Op op, Node *node);
  void prependParameter(Node *node);
  void prependParameter(Op op, Node *node);
  void popFrontParameter();

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
  bool operator==(Node *other);
  virtual Node *clone();
  std::string codeGen();
  std::string toString();
  std::string getIdentifier();

private:
  std::string identifier;
};

class NumberNode : public Node {
public:
  NumberNode(double number) : Node(), number(number) {}
  bool operator==(Node *other);
  virtual Node *clone();
  std::string codeGen();
  std::string toString();
  double getNumber();
  void setNumber(double number);

private:
  static std::ostringstream ss;
  double number;
};

#endif

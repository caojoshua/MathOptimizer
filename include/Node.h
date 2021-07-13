#ifndef NODE_H
#define NODE_H

#include <string>

class Node {
  // TODO: implement toString for each node
  // virtual void toString() = 0;
};

class BinaryOpNode : public Node {
  public:
    enum Op {
      Add,
      Sub,
      Mul,
      Div
    };

    BinaryOpNode(Op op, Node *left, Node *right) : op(op), left(left), right(right) {};
    Op getOp();
    Node *getLeft();
    Node *getRight();

  private:
    Op op;
    Node *left;
    Node *right;
};

class IdentifierNode : public Node {
  public:
    IdentifierNode(std::string identifier) : identifier(identifier) {}
    std::string getIdentifier();

  private:
    std::string identifier;
};

class NumberNode : public Node {
  public:
    NumberNode(float number) : number(number) {}
    float getNumber();

  private:
    float number;
};

#endif

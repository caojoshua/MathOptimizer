#ifndef NODE_H
#define NODE_H

#include <sstream>
#include <string>

class Node {
  public:
    virtual std::string codeGen() = 0;
    virtual std::string toString() = 0;
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
    std::string codeGen();
    std::string toString();
    Op getOp();
    Node *getLeft();
    Node *getRight();
    bool higherPrecedence(BinaryOpNode *other);
    bool samePrecedence(BinaryOpNode *other);
    unsigned getOpPrecedence();
    void unknownOperatorError();

  private:
    Op op;
    Node *left;
    Node *right;

    std::string opToStr();
};

class IdentifierNode : public Node {
  public:
    IdentifierNode(std::string identifier) : identifier(identifier) {}
    std::string codeGen();
    std::string toString();
    std::string getIdentifier();

  private:
    std::string identifier;
};

class NumberNode : public Node {
  public:
    NumberNode(float number) : number(number) {}
    std::string codeGen();
    std::string toString();
    float getNumber();

  private:
    static std::ostringstream ss;
    float number;
};

#endif

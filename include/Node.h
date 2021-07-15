#ifndef NODE_H
#define NODE_H

#include <sstream>
#include <string>

class BinaryOpNode;

class Node {
  public:
    Node() : parent(nullptr) {};
    virtual ~Node() {};
    BinaryOpNode *getParent();
    void setParent(BinaryOpNode *parent);
    virtual std::string codeGen() = 0;
    virtual std::string toString() = 0;

  private:
    BinaryOpNode*parent;

};

class BinaryOpNode : public Node {
  public:
    enum Op {
      Add,
      Sub,
      Mul,
      Div
    };

    BinaryOpNode(Op op, Node *left, Node *right);
    std::string codeGen();
    std::string toString();
    void setOp(Op op);
    void setLeft(Node *left);
    void setRight(Node *right);
    Op getOp();
    Node *getLeft();
    Node *getRight();
    bool higherPrecedence(BinaryOpNode *other);
    bool samePrecedence(BinaryOpNode *other);
    unsigned getOpPrecedence();
    void unknownOperatorError();

    static unsigned getOpPrecedence(Op op);
    static void unknownOperatorError(Op op);

  private:
    Op op;
    Node *left;
    Node *right;

    std::string opToStr();
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

#include "Optimizer.h"
#include "Node.h"
#include <iostream>

Node * optimize(Node *n) {
  BinaryOpNode *binaryOpNode = dynamic_cast<BinaryOpNode *>(n);
  if (!binaryOpNode) {
    return n;
  }

  Node *left = optimize(binaryOpNode->getLeft());
  Node *right = optimize(binaryOpNode->getRight());

  NumberNode *leftNumberNode = dynamic_cast<NumberNode *>(left);
  NumberNode *rightNumberNode = dynamic_cast<NumberNode *>(right);

  if (leftNumberNode && rightNumberNode) {
    BinaryOpNode::Op op = binaryOpNode->getOp();
    delete binaryOpNode;
    return collapseNumberNodes(op, leftNumberNode, rightNumberNode);
  }

  binaryOpNode->setLeft(left);
  binaryOpNode->setRight(right);

   

  return n;
}

NumberNode *collapseNumberNodes(BinaryOpNode::Op op, NumberNode *left, NumberNode *right) {
  float number = 0;
  float leftNumber = left->getNumber();
  float rightNumber = right->getNumber();
  switch (op) {
    case BinaryOpNode::Add:
      number = leftNumber + rightNumber;
      break;
    case BinaryOpNode::Sub:
      number = leftNumber - rightNumber;
      break;
    case BinaryOpNode::Mul:
      number = leftNumber * rightNumber;
      break;
    case BinaryOpNode::Div:
      number = leftNumber / rightNumber;
      break;
    default:
      BinaryOpNode::unknownOperatorError(op);
  }

  return new NumberNode(number);
}

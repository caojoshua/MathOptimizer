#include "Optimizer.h"
#include "Node.h"
#include <iostream>

Node * optimize(Node *n) {
  BinaryOpNode *binaryOpNode = dynamic_cast<BinaryOpNode *>(n);
  if (!binaryOpNode) {
    return n;
  }

  BinaryOpNode::Op op = binaryOpNode->getOp();

  Node *left = optimize(binaryOpNode->getLeft());
  Node *right = optimize(binaryOpNode->getRight());

  NumberNode *leftNumberNode = dynamic_cast<NumberNode *>(left);
  NumberNode *rightNumberNode = dynamic_cast<NumberNode *>(right);
  BinaryOpNode *leftBinaryOpNode = dynamic_cast<BinaryOpNode *>(left);
  BinaryOpNode *rightBinaryOpNode = dynamic_cast<BinaryOpNode *>(right);

  if (leftNumberNode && rightNumberNode) {
    delete binaryOpNode;
    return collapseNumberNodes(op, leftNumberNode, rightNumberNode);
  } else if (leftNumberNode && rightBinaryOpNode && collapseBinaryOpAndNumberNode(op, rightBinaryOpNode, leftNumberNode, true)) {
    return right;
  } else if (rightNumberNode && leftBinaryOpNode && collapseBinaryOpAndNumberNode(op, leftBinaryOpNode, rightNumberNode, false)) {
    return left;
  }

  binaryOpNode->setLeft(left);
  binaryOpNode->setRight(right);

  return n;
}

NumberNode *findNearestCommutativeNumberNode(unsigned precedence, BinaryOpNode *n) {
  if (precedence != n->getOpPrecedence()) {
    return nullptr;
  }

  Node *left = n->getLeft();
  Node *right = n->getRight();

  if (NumberNode *numberNode = dynamic_cast<NumberNode *>(left)) {
    return numberNode;
  } else if (NumberNode *numberNode = dynamic_cast<NumberNode *>(right)) {
    return numberNode;
  }

  BinaryOpNode *leftBinaryOpNode = dynamic_cast<BinaryOpNode *>(left);
  if (leftBinaryOpNode) {
    if (NumberNode *n = findNearestCommutativeNumberNode(precedence, leftBinaryOpNode)) {
      return n;
    }
  }

  BinaryOpNode *rightBinaryOpNode = dynamic_cast<BinaryOpNode *>(left);
  if (rightBinaryOpNode) {
    if (NumberNode *n = findNearestCommutativeNumberNode(precedence, rightBinaryOpNode)) {
      return n;
    }
  }

  return nullptr;
}

void replaceChild(Node *oldChild, Node *newChild) {
  BinaryOpNode *parent = oldChild->getParent();
  if (parent) {
    if (parent->getLeft() == oldChild) {
      parent->setLeft(newChild);
    } else if (parent->getRight() == oldChild) {
      parent->setRight(newChild);
    }
  }
}

void deleteNode(Node *n) {
  BinaryOpNode *parent = n->getParent();
  if (!parent) {
    delete n;
    return;
  }

  BinaryOpNode *grandparent = parent->getParent();

  if (parent->getLeft() == n) {
    replaceChild(parent, parent->getRight());
    parent->setRight(nullptr);
    delete parent;
  } else if (parent->getRight() == n) {
    replaceChild(parent, parent->getLeft());
    parent->setLeft(nullptr);
    delete parent;
  }
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

bool collapseBinaryOpAndNumberNode(BinaryOpNode::Op op, BinaryOpNode *binaryOpNode, NumberNode *numberNode, bool isNumberNodeLeft) {
  Node *left = binaryOpNode->getLeft();
  Node *right = binaryOpNode->getRight();

  NumberNode *collapsableNumberNode = findNearestCommutativeNumberNode(BinaryOpNode::getOpPrecedence(op), binaryOpNode);

  if (!collapsableNumberNode) {
    return false;
  }

  NumberNode *newNumberNode;
  if (isNumberNodeLeft) {
    newNumberNode = collapseNumberNodes(op, numberNode, collapsableNumberNode);
  } else {
    newNumberNode = collapseNumberNodes(op, collapsableNumberNode, numberNode);
  }

  replaceChild(collapsableNumberNode, newNumberNode);
  delete numberNode;
  delete collapsableNumberNode;
  return true;
}

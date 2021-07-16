#include "Optimizer.h"
#include "Node.h"
#include <iostream>

Node *optimize(Node *n) {
  BinaryOpNode *binaryOpNode = dynamic_cast<BinaryOpNode *>(n);
  if (!binaryOpNode) {
    return n;
  }

  binaryOpNode = dynamic_cast<BinaryOpNode *>(foldConstants(binaryOpNode));

  return binaryOpNode;
}

Node *foldConstants(BinaryOpNode *n) {
  BinaryOpNode::Op op = n->getOp();

  Node *left = optimize(n->getLeft());
  Node *right = optimize(n->getRight());

  n->setLeft(left);
  n->setRight(right);

  NumberNode *leftNumberNode = dynamic_cast<NumberNode *>(left);
  NumberNode *rightNumberNode = dynamic_cast<NumberNode *>(right);
  BinaryOpNode *leftBinaryOpNode = dynamic_cast<BinaryOpNode *>(left);
  BinaryOpNode *rightBinaryOpNode = dynamic_cast<BinaryOpNode *>(right);

  if (leftNumberNode && rightNumberNode) {
    delete n;
    return foldNumberNodes(op, leftNumberNode, rightNumberNode);
  } else if (leftNumberNode && rightBinaryOpNode && foldBinaryOpAndNumberNode(op, rightBinaryOpNode, leftNumberNode, true)) {
    return right;
  } else if (rightNumberNode && leftBinaryOpNode && foldBinaryOpAndNumberNode(op, leftBinaryOpNode, rightNumberNode, false)) {
    return left;
  }
  else if (leftBinaryOpNode && rightBinaryOpNode && foldBinaryOpNodes(op, leftBinaryOpNode, rightBinaryOpNode)) {
    return n;
  }

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
    delete parent;
  } else if (parent->getRight() == n) {
    replaceChild(parent, parent->getLeft());
    delete parent;
  }
}

NumberNode *foldNumberNodes(BinaryOpNode::Op op, NumberNode *left, NumberNode *right) {
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

bool foldBinaryOpAndNumberNode(BinaryOpNode::Op op, BinaryOpNode *binaryOpNode, NumberNode *numberNode, bool isNumberNodeLeft) {
  NumberNode *foldableNumberNode = findNearestCommutativeNumberNode(BinaryOpNode::getOpPrecedence(op), binaryOpNode);
  if (!foldableNumberNode) {
    return false;
  }

  NumberNode *newNumberNode;
  if (isNumberNodeLeft) {
    newNumberNode = foldNumberNodes(op, numberNode, foldableNumberNode);
  } else {
    newNumberNode = foldNumberNodes(op, foldableNumberNode, numberNode);
  }

  replaceChild(foldableNumberNode, newNumberNode);
  delete numberNode;
  delete foldableNumberNode;
  return true;
}

bool foldBinaryOpNodes(BinaryOpNode::Op op, BinaryOpNode *left, BinaryOpNode *right) {
  unsigned opPrecedence = BinaryOpNode::getOpPrecedence(op);

  NumberNode *leftFoldableNumberNode = findNearestCommutativeNumberNode(op, left);
  NumberNode *rightFoldableNumberNode = findNearestCommutativeNumberNode(op, right);

  if (!leftFoldableNumberNode && !rightFoldableNumberNode) {
    return false;
  }

  NumberNode *newNumberNode = foldNumberNodes(op, leftFoldableNumberNode, rightFoldableNumberNode);

  replaceChild(leftFoldableNumberNode, newNumberNode);
  deleteNode(rightFoldableNumberNode);
  delete leftFoldableNumberNode;
  return true;
}

#include "Node.h"

BinaryOpNode::Op BinaryOpNode::getOp() {
  return this->op;
}

Node * BinaryOpNode::getLeft() {
  return this->left;
}

Node * BinaryOpNode::getRight() {
  return this->right;
}

std::string IdentifierNode::getIdentifier() {
  return this->identifier;
}

float NumberNode::getNumber() {
  return this->number;
}

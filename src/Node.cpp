#include "Node.h"
#include <iostream>
#include <sstream>

std::string BinaryOpNode::codeGen() {
  std::string op = "";
  switch (this->op) {
    case Add:
      op = "+";
      break;
    case Sub:
      op = "-";
      break;
    case Mul:
      op = "*";
      break;
    case Div:
      op = "/";
      break;
    default:
      std::cerr << "BinaryOpNode has unknown operator " << op << std::endl;
      exit(1);
  }

  return left->codeGen() + ' ' + op + ' ' + right->codeGen();
}

BinaryOpNode::Op BinaryOpNode::getOp() {
  return this->op;
}

Node * BinaryOpNode::getLeft() {
  return this->left;
}

Node * BinaryOpNode::getRight() {
  return this->right;
}

std::string IdentifierNode::codeGen() {
  return this->identifier;
}

std::string IdentifierNode::getIdentifier() {
  return this->identifier;
}

std::string NumberNode::codeGen() {
  std::ostringstream ss;
  ss << this->number;
  return std::string(ss.str());
}

float NumberNode::getNumber() {
  return this->number;
}

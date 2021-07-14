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

  std::string leftString = left->codeGen();
  std::string rightString = right->codeGen();

  BinaryOpNode *leftBinaryOpNode = dynamic_cast<BinaryOpNode *>(left);
  BinaryOpNode *rightBinaryOpNode = dynamic_cast<BinaryOpNode *>(right);

  if (leftBinaryOpNode && this->getOpPrecedence() > leftBinaryOpNode->getOpPrecedence()) {
    leftString = '(' + leftString + ')';
  }

  if (rightBinaryOpNode && this->getOpPrecedence() > rightBinaryOpNode->getOpPrecedence()) {
    rightString = '(' + rightString + ')';
  }

  return leftString + ' ' + op + ' ' + rightString;
}

std::string BinaryOpNode::toString() {
  // TODO: concisely print left and right nodes
  return opToStr();
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

std::string BinaryOpNode::opToStr() {
  switch (this->op) {
    case Add:
      return "+";
      break;
    case Sub:
      return "-";
      break;
    case Mul:
      return "*";
      break;
    case Div:
      return "/";
      break;
    default:
      unknownOperatorError();
  }
  return "";
}

bool BinaryOpNode::higherPrecedence(BinaryOpNode *other) {
  return getOpPrecedence() > other->getOpPrecedence();
}

bool BinaryOpNode::samePrecedence(BinaryOpNode *other) {
  return getOpPrecedence() == other->getOpPrecedence();
}

unsigned BinaryOpNode::getOpPrecedence() {
  switch (this->op) {
    case Add:
    case Sub:
      return 0;
    case Mul:
    case Div:
      return 1;
    default:
      unknownOperatorError();
  }
  return 0;
}

void BinaryOpNode::unknownOperatorError() {
  std::cerr << "BinaryOpNode has unknown operator '" << this->op << "'" << std::endl;
  exit(1);
}

std::string IdentifierNode::codeGen() {
  return this->identifier;
}

std::string IdentifierNode::toString() {
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

std::string NumberNode::toString() {
  return this->codeGen();
}

float NumberNode::getNumber() {
  return this->number;
}

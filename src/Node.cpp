#include "Node.h"
#include <iostream>
#include <sstream>

OperatorNode *Node::getParent() {
  return this->parent;
}

void Node::setParent(OperatorNode *parent) {
  this->parent = parent;
}

OperatorNode::OperatorNode(Precedence precedence, Node *node) : precedence(precedence) {
  this->appendParameter(node);
}

std::string OperatorNode::codeGen() {
  if (parameters.size() == 0) {
    return "";
  }

  auto iter = parameters.cbegin();
  std::string s = childNodeCodeGen(iter->node);
  ++iter;

  while (iter != parameters.cend()) {
    s += ' ' + opToStr(iter->op) + ' ' + childNodeCodeGen(iter->node);
    ++iter;
  }

  return s;
}

std::string OperatorNode::toString() {
  return "not implemented yet";
}

OperatorNode::Precedence OperatorNode::getPrecedence() {
  return this->precedence;
}

std::list<OperatorNode::Parameter> &OperatorNode::getParameters() {
  return parameters;
}

void OperatorNode::appendParameter(Node *node) {
  Op op;
  switch (this->precedence) {
    case SumPrecedence:
      op = Add;
      break;
    case ProductPrecedence:
      op = Mul;
      break;
  }
  parameters.push_back(Parameter{op, node});
}

bool OperatorNode::appendParameter(Op op, Node *node) {
  if (this->precedence != getOpPrecedence(op)) {
    return false;
  }
  parameters.push_back(Parameter{op, node});
  return true;
}

unsigned OperatorNode::getOpPrecedence(Op op) {
  switch (op) {
    case Add:
    case Sub:
      return 0;
    case Mul:
    case Div:
      return 1;
    default:
      OperatorNode::unknownOperatorError(op);
  }
  return 0;
}

std::string OperatorNode::opToStr(Op op) {
  switch (op) {
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
      unknownOperatorError(op);
  }
  return "";
}

std::string OperatorNode::childNodeCodeGen(Node *child) {
  std::string nodeStr = child->codeGen();
  OperatorNode *childOperatorNode = dynamic_cast<OperatorNode *>(child);
  if (childOperatorNode && childOperatorNode->getPrecedence() < this->precedence) {
    nodeStr = '(' + nodeStr + ')';
  }
  return nodeStr;
}

void OperatorNode::unknownOperatorError(Op op) {
  std::cerr << "OperatorNode has unknown operator '" << op << "'" << std::endl;
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

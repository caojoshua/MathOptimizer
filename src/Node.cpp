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
  switch (precedence) {
    case SumPrecedence:
      parameters.push_back(Parameter{Add, node});
      break;
    case ProductPrecedence:
      parameters.push_back(Parameter{Mul, node});
  }
}

std::string OperatorNode::codeGen() {
  if (parameters.size() == 0) {
    return "";
  }

  auto iter = parameters.cbegin();
  std::string s = iter->node->codeGen();
  ++iter;

  while (iter != parameters.cend()) {
    std::string nodeStr = iter->node->codeGen();
    OperatorNode *childOperatorNode = dynamic_cast<OperatorNode *>(iter->node);
    if (childOperatorNode) {
      nodeStr = '(' + nodeStr + ')';
    }
    s += ' ' + opToStr(iter->op) + ' ' + nodeStr;
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

std::list<OperatorNode::Parameter> OperatorNode::getParameters() {
  return parameters;
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

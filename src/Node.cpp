#include "Node.h"
#include <iostream>
#include <sstream>

bool Node::operator==(Node *other) { return false; }

bool Node::operator!=(Node *other) { return !(*this == other); }

OperatorNode *Node::getParent() { return this->parent; }

void Node::setParent(OperatorNode *parent) { this->parent = parent; }

OperatorNode::OperatorNode(Precedence precedence, Node *node)
    : OperatorNode(precedence) {
  this->appendParameter(node);
}

bool OperatorNode::operator==(Node *other) {
  OperatorNode *otherOperatorNode = dynamic_cast<OperatorNode *>(other);
  if (!otherOperatorNode) {
    return false;
  }

  auto thisIter = this->parameters.cbegin();
  auto otherIter = otherOperatorNode->parameters.cbegin();

  // Compare all parameters for both nodes. Order of parameters must be the same
  // for two operator nodes to be equivalent, even if the operator is
  // commutative.
  while (thisIter != this->parameters.cend() &&
         otherIter != otherOperatorNode->parameters.cend()) {
    if (thisIter->op != otherIter->op || *(thisIter->node) != otherIter->node) {
      return false;
    }

    ++thisIter;
    ++otherIter;
  }

  return thisIter == this->parameters.cend() &&
         otherIter == otherOperatorNode->parameters.cend();
}

Node *OperatorNode::clone() {
  OperatorNode *n = new OperatorNode(this->precedence);
  for (Parameter parameter : this->parameters) {
    n->appendParameter(parameter.op, parameter.node->clone());
  }
  return n;
}

bool OperatorNode::sortCompare(Node *other) {
  // TODO: how do we sort OperatorNodes?
  return false;
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

std::string OperatorNode::toString() { return "not implemented yet"; }

OperatorNode::Precedence OperatorNode::getPrecedence() {
  return this->precedence;
}

std::list<OperatorNode::Parameter> &OperatorNode::getParameters() {
  return parameters;
}

bool OperatorNode::appendParameter(Parameter parameter) {
  if (this->precedence != getOpPrecedence(parameter.op)) {
    return false;
  }
  parameters.push_back(parameter);
  return true;
}

void OperatorNode::appendParameter(Node *node) {
  parameters.push_back(Parameter{getDefaultOp(), node});
}

bool OperatorNode::appendParameter(Op op, Node *node) {
  return this->appendParameter(Parameter{op, node});
}

void OperatorNode::prependParameter(Node *node) {
  parameters.push_front(Parameter{getDefaultOp(), node});
}

void OperatorNode::popFrontParameter() { parameters.pop_front(); }

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

OperatorNode::Op OperatorNode::getOppositeOp(Op op) {
  switch (op) {
  case Add:
    return Sub;
  case Sub:
    return Add;
  case Mul:
    return Div;
  case Div:
    return Mul;
  }
  unknownOperatorError(op);
  return Add;
}

OperatorNode::Op OperatorNode::getDefaultOp() {
  switch (this->precedence) {
  case SumPrecedence:
    return Add;
  case ProductPrecedence:
    return Mul;
  }
  return Add;
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
  }
  unknownOperatorError(op);
  return "";
}

std::string OperatorNode::childNodeCodeGen(Node *child) {
  std::string nodeStr = child->codeGen();
  OperatorNode *childOperatorNode = dynamic_cast<OperatorNode *>(child);
  if (childOperatorNode &&
      childOperatorNode->getPrecedence() < this->precedence) {
    nodeStr = '(' + nodeStr + ')';
  }
  return nodeStr;
}

void OperatorNode::unknownOperatorError(Op op) {
  std::cerr << "OperatorNode has unknown operator '" << op << "'" << std::endl;
  exit(1);
}

bool IdentifierNode::operator==(Node *other) {
  IdentifierNode *otherIdentifierNode = dynamic_cast<IdentifierNode *>(other);
  return otherIdentifierNode
             ? this->identifier == otherIdentifierNode->getIdentifier()
             : false;
}

Node *IdentifierNode::clone() { return new IdentifierNode(this->identifier); }

bool IdentifierNode::sortCompare(Node *other) {
  // IdentifierNodes are always to the right of NumberNode
  NumberNode *otherNumberNode = dynamic_cast<NumberNode *>(other);
  if (otherNumberNode) {
    return false;
  }

  // When both nodes are IdentifierNodes, compare their identifier strings
  IdentifierNode *otherIdentifierNode = dynamic_cast<IdentifierNode *>(other);
  if (otherIdentifierNode) {
    return this->identifier < otherIdentifierNode->identifier;
  }

  // IdentifierNodes are always to the left of OperatorNodes
  return true;
}

std::string IdentifierNode::codeGen() { return this->identifier; }

std::string IdentifierNode::toString() { return this->identifier; }

std::string IdentifierNode::getIdentifier() { return this->identifier; }

bool NumberNode::operator==(Node *other) {
  NumberNode *otherNumberNode = dynamic_cast<NumberNode *>(other);
  return otherNumberNode ? this->number == otherNumberNode->getNumber() : false;
}

Node *NumberNode::clone() { return new NumberNode(this->number); }

bool NumberNode::sortCompare(Node *other) {
  // number nodes are always to the left of other node types
  NumberNode *otherNumberNode = dynamic_cast<NumberNode *>(other);
  if (!otherNumberNode) {
    return true;
  }

  // when both nodes are number nodes, compare their numbers
  return this->number < otherNumberNode->number;
}

std::string NumberNode::codeGen() {
  std::ostringstream ss;
  ss << this->number;
  return std::string(ss.str());
}

std::string NumberNode::toString() { return this->codeGen(); }

float NumberNode::getNumber() { return this->number; }

void NumberNode::setNumber(float number) { this->number = number; }

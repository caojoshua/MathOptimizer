#include "Optimizer.h"
#include <iostream>

typedef OperatorNode::Op Op;
typedef OperatorNode::Parameter Parameter;
typedef std::list<Parameter> ParameterList;

float foldFloatAndNumberNode(Op op, float number, NumberNode *numberNode) {
  float right = numberNode->getNumber();
  switch (op) {
    case (OperatorNode::Add):
      return number + right;
    case (OperatorNode::Sub):
      return number - right;
    case (OperatorNode::Mul):
      return number * right;
    case (OperatorNode::Div):
      return number / right;
  }
  return number;
}

void foldNumberNodes(OperatorNode *n) {
  ParameterList &parameters = n->getParameters();
  float number = 0;
  auto iter = parameters.begin();
  while (iter != parameters.end()) {
    if (NumberNode *numberNode = dynamic_cast<NumberNode *>(iter->node)) {
      number = foldFloatAndNumberNode(iter->op, number, numberNode);
      auto next = iter;
      ++next;
      parameters.erase(iter);
      iter = next;
      delete numberNode;
      continue;
    }
    ++iter;
  }
  if (number != 0 || parameters.size() == 0) {
    n->appendParameter(new NumberNode(number));
  }
}

Node *foldConstants(OperatorNode *n) {
  foldNumberNodes(n);
  return n;
}

Node *optimize(Node *n) {
  OperatorNode *operatorNode = dynamic_cast<OperatorNode *>(n);
  if (!operatorNode) {
    return n;
  }
  return foldConstants(operatorNode);
}

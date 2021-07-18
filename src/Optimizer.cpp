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

Node *foldNumberNodes(OperatorNode *n) {
  ParameterList &parameters = n->getParameters();
  float number = n->getPrecedence() == OperatorNode::SumPrecedence ? 0 : 1;

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
  if (parameters.size() == 0) {
    delete n;
    return new NumberNode(number);
  } else if ((n->getPrecedence() == OperatorNode::SumPrecedence && number != 0) ||
      n->getPrecedence() == OperatorNode::ProductPrecedence && number != 1) {
    n->appendParameter(new NumberNode(number));
  }

  // If there is only one parameter left, return the node for reflexive operators. For
  // non-reflexive operators, prepend parameters to maintain correctness.
  if (parameters.size() == 1) {
    if (parameters.front().op == OperatorNode::Sub) {
      n->prependParameter(new NumberNode(0));
    } else if (parameters.front().op == OperatorNode::Div) {
      n->prependParameter(new NumberNode(1));
    } else {
      delete n;
      return parameters.front().node;
    }
  }

  return n;
}

Node *foldConstants(OperatorNode *n) {
  return foldNumberNodes(n);
}

Node *optimize(Node *n) {
  OperatorNode *operatorNode = dynamic_cast<OperatorNode *>(n);
  if (!operatorNode) {
    return n;
  }

  ParameterList &parameters = operatorNode->getParameters();
  auto iter = parameters.begin();
  while (iter != parameters.end()) {
    iter->node = optimize(iter->node);
    ++iter;
  }
  return foldConstants(operatorNode);
}

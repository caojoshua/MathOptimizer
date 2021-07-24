#include "Optimizer.h"
#include <iostream>
#include <map>
#include <vector>

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

void mergeWithChildren(OperatorNode *n) {
  std::list<Parameter> &parameters = n->getParameters();
  std::vector<Parameter> newParameters;

  auto iter = parameters.begin();
  while (iter != parameters.end()) {
    Parameter parameter = *iter;
    OperatorNode *childOperatorNode =
        dynamic_cast<OperatorNode *>(parameter.node);
    if (childOperatorNode &&
        n->getPrecedence() == childOperatorNode->getPrecedence()) {
      for (Parameter newParameter : childOperatorNode->getParameters()) {
        if (parameter.op == OperatorNode::Sub ||
            parameter.op == OperatorNode::Div) {
          newParameter.op = OperatorNode::getOppositeOp(newParameter.op);
        }
        newParameters.push_back(newParameter);
      }

      auto next = iter;
      ++next;
      parameters.erase(iter);
      iter = next;
      continue;
    }
    ++iter;
  }

  for (Parameter newParameter : newParameters) {
    n->appendParameter(newParameter);
  }
}

Node *foldConstants(OperatorNode *n) {
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
  } else if ((n->getPrecedence() == OperatorNode::SumPrecedence &&
              number != 0) ||
             n->getPrecedence() == OperatorNode::ProductPrecedence &&
                 number != 1) {
    n->prependParameter(new NumberNode(number));
  }

  // If there is only one parameter left, return the node for reflexive
  // operators. For non-reflexive operators, prepend parameters to maintain
  // correctness.
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

// Sorts nodes such that they have consistent ordering. This is important when
// we compare terms to be added, so that we can linearly scan for equivalent
// terms.
// ie. x*y+y*x -> x*y->x*y
void sortNodes(Node *n) {
  OperatorNode *operatorNode = dynamic_cast<OperatorNode *>(n);
  if (!n) {
    return;
  }

  operatorNode->getParameters().sort([](Parameter a, Parameter b) {
    // We use an arbitrary sort comparison that can guarentee consistency using
    // the node's codegen. It is possible that two different types of nodes ie.
    // IdentiferNode 'x'
    // OperatorNode with single parameter 'x'
    // In which case its possible the nodes can inconsistently show on the left
    // or right of each other. But in practice, OpertorNodes would with a single
    // parameter would have already been extracted into a single leaf node.
    // NumberNodes and IdentifierNodes should also not conflict, since
    // IdentifierNode strings can never be just a number.
    return a.op < b.op ? true : a.node->codeGen() < b.node->codeGen();
  });
}

NumberNode *getCoefficient(OperatorNode *n) {
  if (n->getParameters().size() == 0) {
    return nullptr;
  }
  // We assume the constant is in the front, and is preceded by a + or *, based
  // on previous computations
  return dynamic_cast<NumberNode *>(n->getParameters().front().node);
}

NumberNode *getCoefficient(Node *n) {
  OperatorNode *operatorNode = dynamic_cast<OperatorNode *>(n);
  if (!operatorNode) {
    return nullptr;
  }
  return getCoefficient(operatorNode);
}

OperatorNode *nodeAsOperatorNode(OperatorNode::Precedence precedence, Node *n) {
  IdentifierNode *identifierNode = dynamic_cast<IdentifierNode *>(n);
  if (identifierNode) {
    return new OperatorNode(precedence, identifierNode);
  }
  return dynamic_cast<OperatorNode *>(n);
}

bool sameVariableTerms(Node *_a, Node *_b) {
  OperatorNode *a = nodeAsOperatorNode(OperatorNode::ProductPrecedence, _a);
  OperatorNode *b = nodeAsOperatorNode(OperatorNode::ProductPrecedence, _b);

  if (!a || !b) {
    return false;
  }

  ParameterList &aParams = a->getParameters();
  ParameterList &bParams = b->getParameters();

  auto aIter = aParams.cbegin();
  auto bIter = bParams.cbegin();

  while (aIter != aParams.cend() && bIter != bParams.cend()) {
    NumberNode *aNumberNode = dynamic_cast<NumberNode *>(aIter->node);
    if (aNumberNode) {
      ++aIter;
      continue;
    }

    NumberNode *bNumberNode = dynamic_cast<NumberNode *>(bIter->node);
    if (bNumberNode) {
      ++bIter;
      continue;
    }

    if (aIter->op != bIter->op || *(aIter->node) != bIter->node) {
      return false;
    }

    ++aIter;
    ++bIter;
  }

  return aIter == aParams.cend() && bIter == bParams.cend();
}

// Fold terms ie. 2*x + 3*x = 5*x
Node *foldTerms(Node *n) {
  OperatorNode *operatorNode = dynamic_cast<OperatorNode *>(n);
  if (!operatorNode ||
      operatorNode->getPrecedence() != OperatorNode::SumPrecedence) {
    return n;
  }

  std::list<Parameter> termCoefficients;
  ParameterList &parameters = operatorNode->getParameters();

  for (Parameter parameter : parameters) {
    Node *childNode = parameter.node->clone();

    // check if node with terms are already in the coefficients map
    Parameter *existingCoefficient = nullptr;
    for (auto &termCoefficient : termCoefficients) {
      if (sameVariableTerms(termCoefficient.node, childNode)) {
        existingCoefficient = &termCoefficient;
        break;
      }
    }
    if (!existingCoefficient) {
      termCoefficients.push_back(Parameter{parameter.op, childNode});
    } else {
      // Compute the coefficient to be added to the term, 1 by default
      int coefficient = 1;
      NumberNode *childNumberNode = getCoefficient(childNode);
      if (childNumberNode) {
        coefficient = childNumberNode->getNumber();
      }

      if (parameter.op == OperatorNode::Sub) {
        coefficient *= -1;
      }

      IdentifierNode *identifierNode =
          dynamic_cast<IdentifierNode *>(existingCoefficient->node);
      OperatorNode *operatorNode =
          dynamic_cast<OperatorNode *>(existingCoefficient->node);

      if (identifierNode) {
        OperatorNode *operatorNode = new OperatorNode(
            OperatorNode::SumPrecedence, new NumberNode(1 + coefficient));
        operatorNode->appendParameter(identifierNode);
        existingCoefficient->node = operatorNode;
      } else if (operatorNode) {
        NumberNode *numberNode = getCoefficient(operatorNode);
        if (numberNode) {
          numberNode->setNumber(numberNode->getNumber() + coefficient);
        } else {
          operatorNode->prependParameter(new NumberNode(1 + coefficient));
        }
      } else {
        // Add a coefficient to the node if it does not already exist
        operatorNode->prependParameter(new NumberNode(coefficient));
      }
    }
  }

  // construct a new node with the terms added up
  OperatorNode *constructedOperatorNode =
      new OperatorNode(OperatorNode::SumPrecedence);
  for (auto &termCoefficient : termCoefficients) {
    constructedOperatorNode->appendParameter(termCoefficient.op,
                                             termCoefficient.node);
  }
  return constructedOperatorNode;
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

  mergeWithChildren(operatorNode);
  sortNodes(operatorNode);
  n = foldConstants(operatorNode);
  n = foldTerms(n);

  return n;
}

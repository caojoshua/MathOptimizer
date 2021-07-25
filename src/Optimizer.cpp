#include "Optimizer.h"
#include <iostream>
#include <map>
#include <vector>

typedef OperatorNode::Op Op;
typedef OperatorNode::Parameter Parameter;
typedef std::list<Parameter> ParameterList;

// Merges n's parameters with with child operator node parameters if the child
// operator node has the same precendence as n
Node *mergeWithChildren(OperatorNode *n) {
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
        newParameters.push_back(
            Parameter{newParameter.op, newParameter.node->clone()});
      }
      iter = parameters.erase(iter);
      delete childOperatorNode;
      continue;
    }
    ++iter;
  }

  for (Parameter newParameter : newParameters) {
    n->appendParameter(newParameter);
  }

  return n;
}

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

// Folds all constants in an operator node
// ie. 2 + x + 3 = 5 + x
// We always leave the constant as the left-most parameter, which is used as an
// assumption for later optimization passes
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
  } else {
    n->prependParameter(new NumberNode(number));
  }

  return n;
}

// Sorts nodes such that they have consistent ordering. This is important when
// we compare terms to be added, so that we can linearly scan for equivalent
// terms.
// ie. x*y+y*x -> x*y->x*y
Node *sortNodes(OperatorNode *n) {
  n->getParameters().sort([](Parameter a, Parameter b) {
    // We use an arbitrary sort comparison that can guarentee consistency using
    // the node's codegen. It is possible that two different types of nodes ie.
    // IdentiferNode 'x' and OperatorNode with single parameter 'x'
    // In which case its possible the nodes can inconsistently show on the left
    // or right of each other. But in practice, OpertorNodes would with a single
    // parameter would have already been extracted into a single leaf node.
    // NumberNodes and IdentifierNodes should also not conflict, since
    // IdentifierNode strings can never be just a number.
    return a.op < b.op ? true : a.node->codeGen() < b.node->codeGen();
  });

  return n;
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

// Helper function to return an identifier node as a single parameter in an
// operator node, or the operator node itself. Number nodes are return as
// nullptr.
OperatorNode *nodeAsOperatorNode(OperatorNode::Precedence precedence, Node *n) {
  IdentifierNode *identifierNode = dynamic_cast<IdentifierNode *>(n);
  if (identifierNode) {
    return new OperatorNode(precedence, identifierNode->clone());
  }
  Node *clone = n->clone();
  OperatorNode *operatorNode = dynamic_cast<OperatorNode *>(clone);
  if (!operatorNode) {
    delete clone;
  }
  return operatorNode;
}

// Checks if _a and _b have the same variable terms
// ie. sameVariableTerms(2*x*(y+z), 3*(y+z)*x) = true
bool sameVariableTerms(Node *_a, Node *_b) {
  OperatorNode *a = nodeAsOperatorNode(OperatorNode::ProductPrecedence, _a);
  OperatorNode *b = nodeAsOperatorNode(OperatorNode::ProductPrecedence, _b);

  if (!a || !b) {
    delete a;
    delete b;
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
      delete a;
      delete b;
      return false;
    }

    ++aIter;
    ++bIter;
  }

  delete a;
  delete b;
  return aIter == aParams.cend() && bIter == bParams.cend();
}

// Fold terms ie. 2*x + 3*x = 5*x
Node *foldTerms(OperatorNode *n) {
  if (n->getPrecedence() != OperatorNode::SumPrecedence) {
    return n;
  }

  std::list<Parameter> termCoefficients;
  ParameterList &parameters = n->getParameters();

  for (Parameter parameter : parameters) {
    Node *childNode = parameter.node;

    // check if node with terms are already in the coefficients map
    Parameter *existingCoefficient = nullptr;
    for (auto &termCoefficient : termCoefficients) {
      if (sameVariableTerms(termCoefficient.node, childNode)) {
        existingCoefficient = &termCoefficient;
        break;
      }
    }

    if (!existingCoefficient) {
      termCoefficients.push_back(Parameter{parameter.op, childNode->clone()});
      continue;
    }

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
        float number = existingCoefficient->op == OperatorNode::Sub
                           ? numberNode->getNumber() * -1
                           : numberNode->getNumber();
        numberNode->setNumber(number + coefficient);
      } else {
        operatorNode->prependParameter(new NumberNode(1 + coefficient));
      }
    } else {
      // Add a coefficient to the node if it does not already exist
      operatorNode->prependParameter(parameter.op, new NumberNode(coefficient));
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

// Fold identity rules
// * x (+|-) 0 = x
// * x (*|/) 1 = x
// * x * 0 = 0
Node *foldIdentities(OperatorNode *n) {
  ParameterList &parameterList = n->getParameters();
  auto iter = parameterList.begin();

  while (iter != parameterList.end()) {
    NumberNode *numberNode = dynamic_cast<NumberNode *>(iter->node);
    if (numberNode) {
      float number = numberNode->getNumber();
      if (number == 0) {
        if (iter->op == OperatorNode::Add || iter->op == OperatorNode::Sub) {
          iter = parameterList.erase(iter);
          continue;
        } else if (iter->op == OperatorNode::Mul) {
          return new NumberNode(0);
        }
      } else if ((iter->op == OperatorNode::Mul ||
                  iter->op == OperatorNode::Div) &&
                 number == 1) {
        iter = parameterList.erase(iter);
        continue;
      }
    }
    ++iter;
  }

  return n;
}

const std::list<Node *(*)(OperatorNode *)> passes{
    mergeWithChildren, sortNodes, foldConstants, foldTerms, foldIdentities,
};

Node *optimize(Node *n) {
  OperatorNode *operatorNode = dynamic_cast<OperatorNode *>(n);

  // We cannot optimize on non OperatorNodes
  if (!operatorNode) {
    return n;
  }

  // First optimize on child nodes
  ParameterList &parameters = operatorNode->getParameters();
  auto iter = parameters.begin();
  while (iter != parameters.end()) {
    iter->node = optimize(iter->node);
    ++iter;
  }

  // Pass the node into optimization passes in order
  for (auto pass : passes) {
    OperatorNode *operatorNode = dynamic_cast<OperatorNode *>(n);
    if (!operatorNode) {
      break;
    }
    n = pass(operatorNode);
  }

  return n;
}

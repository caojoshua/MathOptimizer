#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "Node.h"

Node *optimize(Node *n);
NumberNode *collapseNumberNodes(BinaryOpNode::Op op, NumberNode *left, NumberNode *right);

#endif

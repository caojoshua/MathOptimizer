#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "Node.h"

Node *optimize(Node *n);
NumberNode *findNearestCommutativeNumberNode(unsigned precedence, Node *n);
void replaceChild(Node *oldChild, Node *newChild);
void deleteNode(Node *child);
NumberNode *collapseNumberNodes(BinaryOpNode::Op op, NumberNode *left, NumberNode *right);
bool collapseBinaryOpAndNumberNode(BinaryOpNode::Op op, BinaryOpNode *binaryOpNode, NumberNode *numberNode, bool isNumberNodeLeft);

#endif

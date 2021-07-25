#ifndef TOP_DOWN_PARSER_H
#define TOP_DOWN_PARSER_H

#include "Node.h"
#include "Scanner.h"

Node *parse(std::list<Token> &tokens);

#endif

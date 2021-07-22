#ifndef BOTTOM_UP_PARSER_H
#define BOTTOM_UP_PARSER_H

#include "Node.h"
#include "Scanner.h"

class BottomUpParser {

public:
  BottomUpParser(std::list<Token> tokens) : tokens(tokens){};
  Node *parse();

private:
  struct Symbol {
    Symbol(Token token) : token(token), node(nullptr){};
    Symbol(Node *node) : node(node), token(Token::Unknown){};
    Token token;
    Node *node;
  };

  std::list<Token> tokens;
  std::list<Symbol> symbolStack;

  void parseValue();
  void parseFactor();
  void parseTerm();
  void parseExpression();
  void parseParenthesisExpression();

  Token shift();
  void reduce();
  void unexpectedTokenError();
  void error(std::string msg);
  Token nextToken();
};

#endif

#include "BottomUpParser.h"
#include <iostream>

Node *BottomUpParser::parse() {
  parseValue();
  if (symbolStack.size() != 1) {
    error("more than one symbol on the stack after finishing parsing");
  }
  return symbolStack.front().node;
}

void BottomUpParser::parseValue() {
  Token lookahead = nextToken();
  switch (lookahead.getKind()) {
  case Token::Id:
  case Token::Num:
    shift();
    parseTerm();
    break;
  default:
    parseFactor();
  }
}

void BottomUpParser::parseFactor() {
  parseParenthesisExpression();
  parseTerm();
}

void BottomUpParser::parseTerm() {
  Token lookahead = nextToken();
  switch (lookahead.getKind()) {
  case Token::Mul:
  case Token::Div: {
    shift();
    Token rightParam = nextToken();
    switch (rightParam.getKind()) {
    case Token::Id:
    case Token::Num:
      shift();
      break;
    case Token::LeftParen:
      parseParenthesisExpression();
      break;
    default:
      unexpectedTokenError();
    }
    break;
  }
  case Token::Add:
  case Token::Sub:
    parseExpression();
    return;
  default:
    break;
  }

  reduce();

  lookahead = nextToken();
  switch (lookahead.getKind()) {
  case Token::RightParen:
    reduce();
    return;
  case Token::Mul:
  case Token::Div:
    parseTerm();
    break;
  case Token::Add:
  case Token::Sub:
    parseExpression();
    break;
  default:
    return;
  }
}

void BottomUpParser::parseExpression() {
  Token lookahead = nextToken();

  switch (lookahead.getKind()) {
  case Token::Add:
  case Token::Sub: {
    shift();
    Token rightParam = nextToken();
    switch (rightParam.getKind()) {
    case Token::Id:
    case Token::Num:
      shift();
      break;
    case Token::LeftParen:
      parseParenthesisExpression();
      break;
    default:
      unexpectedTokenError();
    }
  }
  default:
    break;
  }

  lookahead = nextToken();
  switch (lookahead.getKind()) {
  case Token::RightParen:
    reduce();
    return;
  case Token::Add:
  case Token::Sub:
    reduce();
    parseExpression();
    break;
  case Token::Mul:
  case Token::Div:
    parseTerm();
    reduce();
    break;
  default:
    reduce();
    return;
  }
}

void BottomUpParser::parseParenthesisExpression() {
  Token lookahead = nextToken();
  switch (lookahead.getKind()) {
  case Token::LeftParen: {
    // shifting the left parenthesis acts as a marker for the scope
    // of the parenthesis expression
    shift();
    parseValue();
    if (nextToken().getKind() != Token::RightParen) {
      unexpectedTokenError();
    }
    tokens.pop_front();

    // delete the left parenthesis. we shouldn't do this in a
    // a traditional shift-reduce parser, but I'm bad so its OK
    auto iter = symbolStack.begin();
    ++iter;
    if (iter->token.getKind() != Token::LeftParen) {
      error("expected left parenthesis symbol second from the top, but was "
            "some other symbol");
    }
    symbolStack.erase(iter);
  }
  default:
    break;
  }
  return;
}

Token BottomUpParser::shift() {
  Token token = tokens.front();
  switch (token.getKind()) {
  case Token::Id:
    symbolStack.push_front(Symbol{new IdentifierNode(token.getIdentifier())});
    break;
  case Token::Num:
    symbolStack.push_front(Symbol{new NumberNode(token.getNumber())});
    break;
  default:
    symbolStack.push_front(Symbol{token});
  }
  tokens.pop_front();
  return token;
}

void BottomUpParser::reduce() {
  unsigned size = symbolStack.size();
  if (size < 3) {
    return;
  }

  Node *right = symbolStack.front().node;
  symbolStack.pop_front();

  Token op = symbolStack.front().token;
  symbolStack.pop_front();

  Node *left = symbolStack.front().node;
  symbolStack.pop_front();

  if (!left || !right) {
    return;
  }

  OperatorNode::Op operatorNodeOp;
  switch (op.getKind()) {
  case Token::Add:
    operatorNodeOp = OperatorNode::Add;
    break;
  case Token::Sub:
    operatorNodeOp = OperatorNode::Sub;
    break;
  case Token::Mul:
    operatorNodeOp = OperatorNode::Mul;
    break;
  case Token::Div:
    operatorNodeOp = OperatorNode::Div;
    break;
  default:
    return;
  }

  symbolStack.push_front(new OperatorNode(operatorNodeOp, left, right));
}

void BottomUpParser::unexpectedTokenError() { error("unexpected token"); }

void BottomUpParser::error(std::string msg) {
  std::cerr << "fatal: " << msg << std::endl;
  exit(1);
}

Token BottomUpParser::nextToken() {
  if (tokens.size() == 0) {
    return Token::Unknown;
  }
  return tokens.front();
}

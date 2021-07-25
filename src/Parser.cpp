#include "Parser.h"
#include <iostream>

typedef std::list<Token> Tokens;

Node *parseExpression(Tokens &tokens);

void error(std::string msg) {
  std::cerr << "fatal: " << msg << std::endl;
  exit(1);
}

Token parseToken(Tokens &tokens) {
  Token token = tokens.front();
  tokens.pop_front();
  return token;
}

Token mustParseToken(Tokens &tokens, Token::Kind kind) {
  if (tokens.empty()) {
    error("unexpected end of input");
  } else if (!tokens.front().isKind(kind)) {
    error("unexpected token");
  }
  return parseToken(tokens);
}

bool isNextTokenKind(Tokens &tokens, Token::Kind kind) {
  return !tokens.empty() && tokens.front().isKind(kind);
}

Node *parseValue(Tokens &tokens) {
  Node *n = nullptr;

  if (isNextTokenKind(tokens, Token::Id)) {
    n = new IdentifierNode(parseToken(tokens).getIdentifier());
  } else {
    n = new NumberNode(mustParseToken(tokens, Token::Num).getNumber());
  }

  return n;
}

Node *parseFactor(Tokens &tokens) {
  Node *n = nullptr;

  if (isNextTokenKind(tokens, Token::LeftParen)) {
    parseToken(tokens);
    n = parseExpression(tokens);
    mustParseToken(tokens, Token::RightParen);
  } else {
    n = parseValue(tokens);
  }

  return n;
}

Node *parseTerm(Tokens &tokens) {
  Node *n = parseFactor(tokens);
  if (!isNextTokenKind(tokens, Token::Mul) &&
      !isNextTokenKind(tokens, Token::Div)) {
    return n;
  }

  OperatorNode *operatorNode =
      new OperatorNode(OperatorNode::ProductPrecedence, n);
  while (true) {
    if (isNextTokenKind(tokens, Token::Mul)) {
      parseToken(tokens);
      operatorNode->appendParameter(OperatorNode::Mul, parseFactor(tokens));
    } else if (isNextTokenKind(tokens, Token::Div)) {
      parseToken(tokens);
      operatorNode->appendParameter(OperatorNode::Div, parseFactor(tokens));
    } else {
      break;
    }
  }

  return operatorNode;
}

Node *parseExpression(Tokens &tokens) {
  Node *n = parseTerm(tokens);

  if (!isNextTokenKind(tokens, Token::Add) &&
      !isNextTokenKind(tokens, Token::Sub)) {
    return n;
  }

  OperatorNode *operatorNode = new OperatorNode(OperatorNode::SumPrecedence, n);
  while (true) {
    if (isNextTokenKind(tokens, Token::Add)) {
      parseToken(tokens);
      operatorNode->appendParameter(OperatorNode::Add, parseTerm(tokens));
    } else if (isNextTokenKind(tokens, Token::Sub)) {
      parseToken(tokens);
      operatorNode->appendParameter(OperatorNode::Sub, parseTerm(tokens));
    } else {
      break;
    }
  }

  return operatorNode;
}

Node *parse(Tokens &tokens) {
  Node *n = parseExpression(tokens);
  if (!tokens.empty()) {
    error("unexpected token");
  }
  return n;
}

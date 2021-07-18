#ifndef TOP_DOWN_PARSER_H
#define TOP_DOWN_PARSER_H

#include "Node.h"
#include "Scanner.h"

namespace TopDownParser {
typedef std::list<Token> Tokens;

Node *parse(Tokens &tokens);

Node *parseExpression(Tokens &tokens);
Node *parseTerm(Tokens &tokens);
Node *parseFactor(Tokens &tokens);
Node *parseValue(Tokens &tokens);

bool isNextTokenKind(Tokens &tokens, Token::Kind kind);
Token parseToken(Tokens &tokens);
Token mustParseToken(Tokens &tokens, Token::Kind kind);

void error(std::string msg);
} // namespace TopDownParser

#endif

#include "Scanner.h"
#include <iostream>

Token::Kind Token::getKind() { return this->kind; }

float Token::getNumber() { return this->number; }

std::string Token::getIdentifier() { return this->identifier; }

bool Token::isKind(Kind kind) { return this->kind == kind; }

std::list<Token> scan(char *in) {
  std::list<Token> tokens = std::list<Token>();

  while (*in) {
    char c = *in;
    switch (c) {
    case 'a' ... 'z':
    case 'A' ... 'Z': {
      std::string identifier;
      while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        identifier.push_back(c);
        ++in;
        c = *in;
      }
      --in;
      tokens.push_back(Token(Token::Id, identifier));
      break;
    }
    case '0' ... '9': {
      double number = 0;
      while (c >= '0' && c <= '9') {
        number = number * 10 + (c - '0');
        ++in;
        c = *in;
      }
      if (c == '.') {
        // Scan decimal numbers
        ++in;
        c = *in;
        float multiplier = 0.1;
        while (c >= '0' && c <= '9') {
          number = number + (c - '0') * multiplier;
          ++in;
          c = *in;
          multiplier *= 0.1;
        }
      }
      --in;
      tokens.push_back(Token(Token::Num, number));
      break;
    }
    case '+':
      tokens.push_back(Token(Token::Add));
      break;
    case '-':
      tokens.push_back(Token(Token::Sub));
      break;
    case '*':
      tokens.push_back(Token(Token::Mul));
      break;
    case '/':
      tokens.push_back(Token(Token::Div));
      break;
    case '(':
      tokens.push_back(Token(Token::LeftParen));
      break;
    case ')':
      tokens.push_back(Token(Token::RightParen));
      break;
    case ' ':
      break;
    default:
      std::cerr << "fatal: unexpected character '" << c << "'" << std::endl;
      exit(1);
    }
    ++in;
  }

  return tokens;
}

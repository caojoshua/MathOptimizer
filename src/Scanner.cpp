#include "Scanner.h"
#include <iostream>

float Token::getNumber() {
  return number;
}

std::string Token::getIdentifier() {
  return identifier;
}

std::vector<Token> scan(char *in) {
  std::vector<Token> tokens = std::vector<Token>();

  while (*in) {
    char c = *in;
    switch (c) {
      case 'a' ... 'z':
      case 'A' ... 'Z':
      {
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
      case '0' ... '9':
      {
        float number = 0;
        while (c >= '0' && c <= '9') {
          number = number * 10 + (c - '0');
          ++in;
          c = *in;
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
    }
    ++in;
  }

  return tokens;
}

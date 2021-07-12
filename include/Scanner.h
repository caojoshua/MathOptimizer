#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>

class Token {
  public:
    enum Kind {
      Id,
      Num,
      Add,
      Sub,
      Mul,
      Div,
      LeftParen,
      RightParen,
    };

    Token(Kind kind) : kind(kind) {};
    Token(Kind kind, float number) : kind(kind), number(number) {};
    Token(Kind kind, std::string identifier) : kind(kind), identifier(identifier) {};
    float getNumber();
    std::string getIdentifier();

  private:
    Kind kind;
    float number;
    std::string identifier;
};

std::vector<Token> scan(char *in);

#endif

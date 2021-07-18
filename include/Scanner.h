#ifndef SCANNER_H
#define SCANNER_H

#include <list>
#include <string>

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

  Token(Kind kind) : kind(kind){};
  Token(Kind kind, float number) : kind(kind), number(number){};
  Token(Kind kind, std::string identifier)
      : kind(kind), identifier(identifier){};
  Kind getKind();
  float getNumber();
  std::string getIdentifier();
  bool isKind(Kind kind);

private:
  Kind kind;
  float number;
  std::string identifier;
};

std::list<Token> scan(char *in);

#endif

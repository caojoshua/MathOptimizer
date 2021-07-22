#include "BottomUpParser.h"
#include "Optimizer.h"
#include "Scanner.h"
#include "TopDownParser.h"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::cerr << "fatal: no input" << std::endl;
    return 1;
  } else if (argc > 2) {
    std::cout << "ignoring all arguments after the first" << std::endl;
  }

  std::list<Token> tokens = scan(argv[1]);
  std::list<Token> bottomUpTokens = std::list<Token>(tokens);

  Node *topDownOutput = optimize(TopDownParser::parse(tokens));
  std::cout << "TopDown output: " << std::endl << topDownOutput->codeGen() << std::endl << std::endl;

  BottomUpParser bottomUpParser(bottomUpTokens);
  Node *bottomUpOutput = optimize(bottomUpParser.parse());
  std::cout << "BottomUp output: " << std::endl << bottomUpOutput->codeGen() << std::endl;

  return 0;
}

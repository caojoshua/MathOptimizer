#include "Optimizer.h"
#include "Scanner.h"
#include "Parser.h"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::cerr << "fatal: no input" << std::endl;
    return 1;
  } else if (argc > 2) {
    std::cout << "ignoring all arguments after the first" << std::endl;
  }

  std::list<Token> tokens = scan(argv[1]);

  Node *ast = parse(tokens);

  ast = optimize(ast);

  std::cout << ast->codeGen() << std::endl;

  delete ast;

  return 0;
}

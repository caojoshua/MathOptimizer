/* #include "Optimizer.h" */
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

  Node * topDownOutput = TopDownParser::parse(tokens);

  /* topDownOutput = optimize(topDownOutput); */

  std::cout << topDownOutput->codeGen() << std::endl;

  return 0;
}

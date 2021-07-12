#include "Scanner.h"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::cerr << "fatal: no input" << std::endl;
    return 1;
  } else if (argc > 2) {
    std::cout << "ignoring all arguments after the first" << std::endl;
  }

  scan(argv[1]);

  return 0;
}

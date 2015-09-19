#include <stdio.h>
#include <vector>
#include <iostream>

#include "shell.h"

int main() {
  Shell shell;
  int result_code = shell.run(std::cin, std::cout, std::cerr);
  printf("Shell returned: %d", result_code);
  return 0;
}

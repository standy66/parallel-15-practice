#include <stdio.h>
#include <vector>

#include "shell.h"

int main() {
  Shell shell;
  int result_code = shell.run();
  printf("Shell returned: %d", result_code);
  return 0;
}

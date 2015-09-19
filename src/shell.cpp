#include "shell.h"

#include <string>

#include "dbg.h"

int Shell::run(std::istream& in, std::ostream& out, std::ostream& err) {
  std::string line;
  while (std::getline(in, line)) {
    DBG("Got line: " << line);


  }
  return 0;
}

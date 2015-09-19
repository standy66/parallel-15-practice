#pragma once

#include <istream>
#include <ostream>

class Shell {
public:
  int run(std::istream& in, std::ostream& out, std::ostream& err);
};

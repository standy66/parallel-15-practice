#pragma once

#include <istream>
#include <ostream>
#include <map>

class Shell {
public:
  typedef int (*Action) (std::istream& argument_stream, std::ostream& out,
                         std::ostream& err);

  Shell(std::map<std::string, Action> actionMap);
  Shell();

  int run(std::istream& in, std::ostream& out, std::ostream& err);

private:
  std::map<std::string, Action> actionMap;
};

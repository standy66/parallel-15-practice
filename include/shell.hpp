#pragma once

#include <istream>
#include <ostream>
#include <map>
#include <stdexcept>

class ShellException: public std::runtime_error {
public:
  explicit ShellException(const std::string& what);
};

class Shell {
public:
  typedef void (*Action) (std::istream& argument_stream, std::ostream& out,
                         std::ostream& err);

  Shell(const std::map<std::string, Action>& actionMap);
  Shell();

  int run(std::istream& in, std::ostream& out, std::ostream& err);

private:
  std::map<std::string, Action> actionMap;
};

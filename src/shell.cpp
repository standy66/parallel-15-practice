#include "shell.hpp"

#include <string>
#include <sstream>

#include "dbg.hpp"

ShellException::ShellException(const std::string& what)
  : std::runtime_error(what) { }

Shell::Shell() : actionMap() { }

Shell::Shell(const std::map<std::string, Shell::Action>& map)
  : actionMap(map) { }

int Shell::run(std::istream& in, std::ostream& out, std::ostream& err) {
  std::string line;
  while (std::getline(in, line)) {
    std::stringstream linestream;
    linestream << line;
    std::string commandName;
    linestream >> commandName;
    Action action = actionMap[commandName];
    if (action != NULL) {
      try {
        action(linestream, out, err);
      } catch (ShellException& e) {
        err << e.what();
      }
    } else {
      err << "Command " << commandName << " not found" << std::endl;
    }
  }
  return 0;
}

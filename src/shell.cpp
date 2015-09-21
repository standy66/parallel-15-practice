#include "shell.hpp"

#include <string>
#include <sstream>

#include "dbg.hpp"
#include "exceptions.hpp"

Shell::Shell() : actionMap() { }

Shell::Shell(const std::map<std::string, Shell::Action>& map)
  : actionMap(map) { }

int Shell::run(std::istream& in, std::ostream& out, std::ostream& err) const {
  std::string line;
  while (std::getline(in, line)) {
    Args args = extractArgs(line);
    if (args.size() == 0) {
      continue;
    }
    ActionMap::const_iterator iter = actionMap.find(args[0]);
    if (iter != actionMap.end()) {
      Action action = iter->second;
      try {
        action(args, out);
      } catch (BasicException& e) {
        err << e.what() << std::endl;
      }
    } else {
      err << "Command " << args[0] << " not found" << std::endl;
    }
  }
  return 0;
}

Shell::Args Shell::extractArgs(const std::string& line) const {
  std::istringstream linestream(line);
  Args result;
  std::string arg;
  while (linestream >> arg) {
    result.push_back(arg);
  }
  return result;
}

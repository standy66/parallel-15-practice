#include "shell.h"

#include <string>
#include <sstream>
  
#include "dbg.h"

Shell::Shell() : actionMap() { }

Shell::Shell(std::map<std::string, Shell::Action> map)
  : actionMap(map) { }

int Shell::run(std::istream& in, std::ostream& out, std::ostream& err) {
  std::string line;
  while (std::getline(in, line)) {
    //DBG("Got line: " << line);
    std::stringstream linestream;
    linestream << line;
    std::string commandName;
    linestream >> commandName;
    //DBG("Command name: " << commandName);
    Action action = actionMap[commandName];
    if (action != 0) {
      int resultCode = action(linestream, out, err);
      if (resultCode != 0) {
        err << "Command " << commandName << " exited with code " << resultCode << std::endl;
      }
    } else {
      err << "Command " << commandName << " not found" << std::endl;
    }
  }
  return 0;
}

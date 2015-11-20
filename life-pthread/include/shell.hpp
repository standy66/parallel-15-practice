#pragma once

#include <istream>
#include <ostream>
#include <map>
#include <vector>

class Shell {
public:
  typedef std::vector<std::string> Args;
  typedef void (*Action) (const Args& args, std::ostream& out);

  Shell(const std::map<std::string, Action>& actionMap);
  Shell();

  int run(std::istream& in, std::ostream& out, std::ostream& err) const;

private:
  typedef std::map<std::string, Action> ActionMap;
  std::map<std::string, Action> actionMap;

  Args extractArgs(const std::string& line) const;
};

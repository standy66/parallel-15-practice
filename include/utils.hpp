#pragma once

#include <string>
#include <vector>

namespace utility {

class NonCopyable {
public:
  NonCopyable() { }
private:
  NonCopyable(NonCopyable&) { }
  NonCopyable(const NonCopyable&) { }
  NonCopyable& operator=(const NonCopyable&) { }
};

std::string toString(int value);
std::string toString(const std::vector<std::vector<bool> >& vec);
std::string toString(const std::vector<bool>& vec);

int intVal(const std::string& str);

} //end of namespace utility

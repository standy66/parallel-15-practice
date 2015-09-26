#pragma once

#include <string>

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

int intVal(const std::string& str);

} //end of namespace utility

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
  NonCopyable& operator=(const NonCopyable&) { return *this; }
};

std::string toString(int value);
std::string toString(const std::vector<std::vector<bool> >& vec);
std::string toString(const std::vector<bool>& vec);

int intVal(const std::string& str);

size_t packSize(size_t width, size_t height);
void pack(const std::vector<std::vector<bool> >& vec, int from, int to, char* dest, size_t& len);
void pack(const std::vector<std::vector<bool> >& vec, char* dest, size_t& len);
std::vector<std::vector<bool> > unpack(char* mem);

} //end of namespace utility

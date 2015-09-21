#include "utils.hpp"

#include <sstream>

#include "exceptions.hpp"

namespace utility {

std::string toString(int value) {
  std::ostringstream temp;
  temp << value;
  return temp.str();
}

int intVal(const std::string& str) {
  std::istringstream temp(str);
  int result;
  if (temp >> result) {
    return result;
  } else {
    throw ConversionException("error converting " + str + " to int");
  }
}

} // end of namespace utility

#include "utils.hpp"

#include <sstream>

#include "exceptions.hpp"

namespace utility {

std::string toString(int value) {
  std::ostringstream temp;
  temp << value;
  return temp.str();
}

std::string toString(const std::vector<std::vector<bool> >& vec) {
  std::ostringstream temp;
  for (size_t i = 0; i < vec.size(); ++i) {
    for (size_t j = 0; j < vec[i].size(); ++j) {
      temp << vec[i][j];
    }
    temp << std::endl;
  }
  return temp.str();
}

std::string toString(const std::vector<bool>& vec) {
  std::ostringstream temp;
  for (size_t i = 0; i < vec.size(); ++i) {
    temp << vec[i];
  }
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

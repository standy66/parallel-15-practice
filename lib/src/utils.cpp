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

size_t packSize(size_t width, size_t height) {
  size_t chars_in_row = (height + 7) / 8;
  return 2 * sizeof(size_t) + chars_in_row * width;
}


void pack(const std::vector<std::vector<bool> >& vec, size_t from, size_t to, char* dest, size_t& len) {
  size_t width = to - from;
  size_t height = vec[0].size();
  size_t* casted = (size_t*)dest;
  casted[0] = width;
  casted[1] = height;
  dest += 2 * sizeof(size_t);
  size_t chars_in_row = (height + 7) / 8;
  for (size_t i = from; i < to; ++i) {
    for (size_t j = 0; j < chars_in_row; ++j) {
      char c = 0;
      for (size_t l = 0; l < 8 && 8 * j + l < height; ++l) {
        if (vec[i][8 * j + l]) {
          c |= (1 << l);
        }
      }
      dest[i * chars_in_row + j] = c;
    }
  }
  len = 2 * sizeof(size_t) + chars_in_row * width;
}


void pack(const std::vector<std::vector<bool> >& vec, char* dest, size_t& len) {
  pack(vec, 0, vec.size(), dest, len);
}

std::vector<std::vector<bool> > unpack(char* mem) {
  size_t* casted = (size_t*)mem;
  size_t width = casted[0];
  size_t height = casted[1];
  mem += 2 * sizeof(size_t);
  size_t chars_in_row = (height + 7) / 8;
  std::vector<std::vector<bool> > result(width, std::vector<bool>(height, false));
  for (size_t i = 0; i < width; ++i) {
    for (size_t j = 0; j < height; ++j) {
      size_t off = j % 8;
      size_t ind = j / 8;
      result[i][j] = ((mem[i * chars_in_row + ind] >> off) & 1);
    }
  }
  return result;
}

} // end of namespace utility

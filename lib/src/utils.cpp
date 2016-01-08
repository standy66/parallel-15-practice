#include "utils.hpp"

#include <sstream>

#include <time.h>
#include <sys/time.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

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


void pack(const std::vector<std::vector<bool> >& vec, int from, int to, char* dest, size_t& len) {
  size_t width = to - from;
  size_t height = vec[0].size();
  size_t* casted = (size_t*)dest;
  casted[0] = width;
  casted[1] = height;
  dest += 2 * sizeof(size_t);
  size_t chars_in_row = (height + 7) / 8;
  for (int i = from; i < to; ++i) {
    for (size_t j = 0; j < chars_in_row; ++j) {
      char c = 0;
      for (size_t l = 0; l < 8 && 8 * j + l < height; ++l) {
        if (vec[(i + vec.size()) % vec.size()][8 * j + l]) {
          c |= (1 << l);
        }
      }
      dest[(i - from) * chars_in_row + j] = c;
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

double rtClock() {
  struct timespec ts;

  #ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  ts.tv_sec = mts.tv_sec;
  ts.tv_nsec = mts.tv_nsec;

  #else
  clock_gettime(CLOCK_REALTIME, &ts);
  #endif

  double res = ts.tv_sec;
  res += ts .tv_nsec * 1e-9;
  return res;
}

} // end of namespace utility

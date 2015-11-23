#pragma once

#include <iostream>
#include "threading.hpp"


//TODO: refactor this

extern Mutex m;

#ifdef NDEBUG
#define DBG(x) do {} while(0)
#else
#define DBG(x) do { m.lock(); std::cerr << x << std::endl; m.unlock(); } while (0)
#endif

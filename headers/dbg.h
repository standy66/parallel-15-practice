#pragma once

#include <iostream>

#ifndef DEBUG
#define DBG(x) do {} while(0)
#else
#define DBG(x) do { std::cerr << x << std::endl; } while (0)
#endif

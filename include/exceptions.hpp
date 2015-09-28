#pragma once

#include <stdexcept>
#include <errno.h>
#include <string.h>

#include "utils.hpp"

class BasicException: public std::runtime_error {
public:
  BasicException(const std::string& what)
    : std::runtime_error(what) { }
};

class ShellSilentInterruptException: public BasicException {
public:
  ShellSilentInterruptException(const std::string& what)
    : BasicException(what) { }
};

class ActionException: public BasicException {
public:
  ActionException(const std::string& what)
    : BasicException(what) { }
};

class ConversionException: public BasicException {
public:
  ConversionException(const std::string& what)
    : BasicException(what) { }
};

class IllegalArgumentException: public BasicException {
public:
  IllegalArgumentException(const std::string& what)
    : BasicException(what) { }
};

class LegacyException: public BasicException {
public:
  LegacyException(const std::string& what)
    : BasicException(what) { }
};


#define THROW_LEGACY_EXCEPTION throw LegacyException( \
  utility::toString(__LINE__) + std::string(__FILE__) + std::string(strerror(errno)));

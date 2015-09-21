#pragma once

#include <stdexcept>

class BasicException: public std::runtime_error {
public:
  BasicException(const std::string& what)
    : std::runtime_error(what) { }
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

class LegacyException: public BasicException {
public:
  LegacyException(const std::string& what)
    : BasicException(what) { }
};

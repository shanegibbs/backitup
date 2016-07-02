#ifndef LOG_H
#define LOG_H

#include <exception>
#include <iostream>
#include <sstream>
#include <stdarg.h>
#include <stdexcept>
#include <stdio.h>

namespace backitup {

enum LogLevel { DEBUG, INFO, WARN, ERROR, FATAL, NONE };

extern LogLevel loglevel;

class FatalLogEvent : public std::runtime_error {
 public:
  FatalLogEvent() : std::runtime_error("FatalLogEvent") {}
};

class Log {
 public:
  Log(const std::string name) : _name(name) {}
  const std::string &name() const { return _name; }

  static void setup();

  static FILE *out;

 private:
  const std::string _name;
  static bool abort_on_fatal;
};

class Logger {
 public:
  Logger(Log &parent, LogLevel level) : _parent(parent), _level(level) {
    _buffer << _parent.name() << " ";

    std::string l;
    switch (_level) {
      case DEBUG:
        l = "\x1b[36mDEBUG\x1b[0m";
        break;

      case INFO:
        l = "\x1b[34mINFO\x1b[0m ";
        break;

      case WARN:
        l = "\x1b[33mWARN\x1b[0m ";
        break;

      case ERROR:
        l = "\x1b[31mERROR\x1b[0m";
        break;

      case FATAL:
        l = "\x1b[31mFATAL\x1b[0m";
        break;

      case NONE:
        break;
    };

    _buffer << l << " ";
  }

  ~Logger() {
    _buffer << std::endl;
    std::cerr << _buffer.str();
  }

  template <typename T>
  Logger &operator<<(T const &val) {
    _buffer << val;
    return *this;
  }

 private:
  Log &_parent;
  LogLevel _level;
  std::ostringstream _buffer;
};

#define debug           \
  if (DEBUG < loglevel) \
    ;                   \
  else                  \
  Logger(LOG, DEBUG)

#define info           \
  if (INFO < loglevel) \
    ;                  \
  else                 \
  Logger(LOG, INFO)

#define warn           \
  if (WARN < loglevel) \
    ;                  \
  else                 \
  Logger(LOG, WARN)

#define error           \
  if (ERROR < loglevel) \
    ;                   \
  else                  \
  Logger(LOG, ERROR)

#define fatal           \
  if (FATAL < loglevel) \
    ;                   \
  else                  \
  Logger(LOG, FATAL)

int assert_fail(const char *str, const char *func, const char *file, int line);
}

#endif

#ifndef LOG_H
#define LOG_H

#include <ctime>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdarg.h>
#include <stdexcept>
#include <stdio.h>

namespace backitup {

enum LogLevel { DEBUG, INFO, WARN, ERROR, FATAL, NONE };

// aparently stdout isn't thread safe. who knew!
static std::mutex out_mutex;

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
    std::time_t t = std::time(nullptr);
    char mbstr[100];
    if (std::strftime(mbstr, sizeof(mbstr), "%F %T", std::localtime(&t))) {
      _buffer << mbstr << ' ';
    }

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
    std::lock_guard<std::mutex> lock(out_mutex);
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

#define debug                               \
  if (backitup::DEBUG < backitup::loglevel) \
    ;                                       \
  else                                      \
  backitup::Logger(LOG, backitup::DEBUG)

#define info                               \
  if (backitup::INFO < backitup::loglevel) \
    ;                                      \
  else                                     \
  backitup::Logger(LOG, backitup::INFO)

#define warn                               \
  if (backitup::WARN < backitup::loglevel) \
    ;                                      \
  else                                     \
  backitup::Logger(LOG, backitup::WARN)

#define error                               \
  if (backitup::ERROR < backitup::loglevel) \
    ;                                       \
  else                                      \
  backitup::Logger(LOG, backitup::ERROR)

#define fatal                               \
  if (backitup::FATAL < backitup::loglevel) \
    ;                                       \
  else                                      \
  backitup::Logger(LOG, backitup::FATAL)

int assert_fail(const char *str, const char *func, const char *file, int line);
}

#endif

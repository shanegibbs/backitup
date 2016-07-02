#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Log.h"

using namespace std;

namespace backitup {

LogLevel loglevel = NONE;

FILE *Log::out = stderr;
bool Log::abort_on_fatal = true;

void Log::setup() {
  char *log_level = getenv("LOG_LEVEL");

  if (log_level) {
    if (strcmp(log_level, "debug") == 0) {
      loglevel = DEBUG;
    } else if (strcmp(log_level, "info") == 0) {
      loglevel = INFO;
    } else if (strcmp(log_level, "warn") == 0) {
      loglevel = WARN;
    } else if (strcmp(log_level, "error") == 0) {
      loglevel = ERROR;
    } else if (strcmp(log_level, "fatal") == 0) {
      loglevel = FATAL;
    }
  }
}

int assert_fail(const char *str, const char *func, const char *file, int line) {
  fprintf(stderr,
          "\nAssertion failed at %s:%d:%s\n\nAssertion that failed was "
          "[\x1b[33m%s\x1b[0m]\n\n",
          file, line, func, str);
  abort();
}
}

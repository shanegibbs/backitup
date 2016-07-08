#ifndef HELPERS_H_
#define HELPERS_H_

#include <string>

namespace backitup {

string trim_slashes(string s) {
  if (s.front() == '/') s = s.substr(1);
  if (s.back() == '/') s = s.substr(0, s.size() - 1);
  return s;
}
}

#endif

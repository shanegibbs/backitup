#ifndef INDEX_H_
#define INDEX_H_

#include "Node.h"

namespace backitup {

class Index {
 public:
  virtual bool contains(const Node &n) = 0;
  virtual void save(const Node &n) = 0;
  virtual void deleted(const Node &n, long mtime) = 0;
  virtual NodeList latest(const string &path) = 0;
  virtual void flush() = 0;
};
}

#endif
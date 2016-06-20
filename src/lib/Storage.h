#ifndef STORAGE_H_
#define STORAGE_H_

#include "Node.h"

namespace backitup {

class Storage {
 public:
  virtual void send(const string& base_path, Node& n) = 0;
};
}

#endif
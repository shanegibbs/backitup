#ifndef BACKITUPX_H_
#define BACKITUPX_H_

#include <string>

#include "BackupPath.h"
#include "Index.h"
#include "Storage.h"

namespace backitup {

class Backitup {
 public:
  Backitup(Index& i, Storage& s);

  void run(BackupPath& b);

 private:
  std::string _base_path;
  Index& _index;
  Storage& _store;
};
}

#endif /* BACKITUP_H_ */

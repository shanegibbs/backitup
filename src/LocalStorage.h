#ifndef LOCAL_STORAGE_H_
#define LOCAL_STORAGE_H_

#include <memory>

#include "Node.h"

namespace backitup {

class LocalStorage {
 public:
  static std::shared_ptr<LocalStorage> create(const std::string& path) {
    return std::shared_ptr<LocalStorage>(new LocalStorage(path));
  }

  LocalStorage(const std::string& path);
  void send(const string& base_path, Node &n);

 private:
  std::string _path;
};
}

#endif

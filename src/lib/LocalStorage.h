#ifndef LOCAL_STORAGE_H_
#define LOCAL_STORAGE_H_

#include <memory>

#include "Node.h"
#include "Storage.h"

namespace backitup {

class LocalStorageException : public std::runtime_error {
 public:
  LocalStorageException(std::string msg)
      : runtime_error(
            string(string("LocalStorageException: ").append(msg)).c_str()) {}
};

class LocalStorage : public Storage {
 public:
  static std::shared_ptr<LocalStorage> create(const std::string& path) {
    return std::shared_ptr<LocalStorage>(new LocalStorage(path));
  }

  LocalStorage(const std::string& path);
  void send(const string& base_path, Node& n);

 private:
  std::string _path;
};
}

#endif

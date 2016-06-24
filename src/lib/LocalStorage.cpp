#include "LocalStorage.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sys/stat.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "HashFuncs.h"

using namespace std;

namespace backitup {

mutex store_mutex;

LocalStorage::LocalStorage(const std::string& path) : _path(path) {
  cout << "Backing up to: " << path << endl;

  if (boost::filesystem::exists(path)) return;

  if (mkdir(path.c_str(), 0755) == -1) {
    cout << "Failed to mkdir for storage" << endl;
    // exit(EXIT_FAILURE);
  }
}

void LocalStorage::send(const string& base_path, Node& n) {
  int bufLen = 4096;
  char buf[bufLen];

  Hash hash;

  lock_guard<mutex> guard(store_mutex);

  string tmp_path = _path + "/_";
  ofstream tmp_stream(tmp_path, ios::out | ios::binary | ios::trunc);
  if (!tmp_stream.is_open()) {
    throw LocalStorageException("Failed to open tmp file: " + tmp_path);
  }

  string source = base_path + n.path() + "/" + n.getName();

  ifstream file(source, ios::in | ios::binary);
  if (!file.is_open()) {
    throw LocalStorageException("Failed to open: " + source);
  }

  while (file) {
    file.read(buf, bufLen);
    hash.update(buf, file.gcount());
    // tmp_stream.write(buf, file.gcount());
    if (file.eof()) break;
  }

  file.close();
  tmp_stream.close();

  string hash_str = hash.get();
  n.sha256(hash_str);

  string final_name = _path + "/" + hash_str;
  if (boost::filesystem::exists(final_name)) {
    // cout << "LocalStorage already store: " << hash_str << endl;
    remove(tmp_path.c_str());
    return;
  }
  // cout << "LocalStorage storing: " << hash_str << endl;

  if (rename(tmp_path.c_str(), final_name.c_str()) != 0) {
    throw LocalStorageException("Failed to rename file " + tmp_path + " to " +
                                final_name + ": " + strerror(errno));
  }
}
}

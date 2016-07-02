#include "LocalStorage.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sys/stat.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "HashFuncs.h"
#include "Log.h"

using namespace std;

namespace backitup {

static Log LOG = Log("LocalStorage");

mutex store_mutex;

LocalStorage::LocalStorage(const std::string& path) : _path(path) {
  info << "Backing up to " << path;

  if (boost::filesystem::exists(path)) return;

  debug << "Creating directory " << path;
  if (mkdir(path.c_str(), 0755) == -1) {
    fatal << "Failed to mkdir for storage";
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

  string source = base_path + "/" + n.path() + "/" + n.getName();

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

  debug << "Calculated hash for " << source << " as " << hash_str;

  string final_name = _path + "/" + hash_str;
  if (boost::filesystem::exists(final_name)) {
    // cout << "LocalStorage already store: " << hash_str << endl;
    remove(tmp_path.c_str());
    return;
  }

  info << "Storing: " << hash_str;

  if (rename(tmp_path.c_str(), final_name.c_str()) != 0) {
    throw LocalStorageException("Failed to rename file " + tmp_path + " to " +
                                final_name + ": " + strerror(errno));
  }
}
}

#include "LocalStorage.h"

#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <cstdio>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "HashFuncs.h"

namespace backitup {

LocalStorage::LocalStorage(const std::string& path) : _path(path) {
  cout << "Setting up storage at " << path << endl;

  if (boost::filesystem::exists(path)) return;

  if (mkdir(path.c_str(), 0755) == -1) {
    cout << "Failed to mkdir for storage" << endl;
    // exit(EXIT_FAILURE);
  }
}

void LocalStorage::send(const string& base_path, Node& n) {
  int bufLen = 4096;
  char buf[bufLen];

  // read into buffer
  // add to hash

  // write out to storage

  // n.sha256(sha256(base_path + n.getFullPath()));

  Hash hash;

  string tmp_path = _path + "/_";
  ofstream tmp_stream(tmp_path, ios::out | ios::binary | ios::trunc);
  if (!tmp_stream.is_open()) {
    cout << "ERROR failed to open " << tmp_path << endl;
  }

  string source = base_path + n.getFullPath();
  cout << "Sending " << source << endl;

  ifstream file(source, ios::in | ios::binary);
  if (!file.is_open()) {
    cout << "ERROR failed to open " << source << endl;
  }

  while (file) {
    file.read(buf, bufLen);
    hash.update(buf, file.gcount());
    tmp_stream.write(buf, file.gcount());
    if (file.eof()) break;
  }

  file.close();
  tmp_stream.close();

  string hash_str = hash.get();
  n.sha256(hash_str);

  string final_name = _path + "/" + hash_str;
  if (boost::filesystem::exists(final_name)) {
    return;
  }

  cout << "Storing " << n.getFullPath() << endl;
  rename(tmp_path.c_str(), final_name.c_str());

  cout << hash_str << endl;
}
}

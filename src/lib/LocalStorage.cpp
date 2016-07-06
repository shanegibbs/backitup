#include "LocalStorage.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sys/stat.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>

#include "HashFuncs.h"
#include "Log.h"

using namespace std;
using namespace boost::iostreams;
using namespace boost::filesystem;

namespace backitup {

static Log LOG = Log("LocalStorage");

mutex store_mutex;

LocalStorage::LocalStorage(const std::string& path) : _path(path) {
  info << "Local storage path: " << path;

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

  string tmp_path_bz = _path + "/_.bz2";
  ofstream tmp_stream_bz(tmp_path_bz, ios::out | ios::binary | ios::trunc);
  if (!tmp_stream_bz.is_open()) {
    throw LocalStorageException("Failed to open tmp file: " + tmp_path_bz);
  }

  string source = base_path + "/" + n.path() + "/" + n.getName();

  ifstream file(source, ios::in | ios::binary);
  if (!file.is_open()) {
    throw LocalStorageException("Failed to open: " + source);
  }

  filtering_streambuf<output> out_filter;
  out_filter.push(bzip2_compressor());
  out_filter.push(tmp_stream_bz);
  ostream tmp_stream_bz_out(&out_filter);

  while (file) {
    file.read(buf, bufLen);
    hash.update(buf, file.gcount());
    string bytes(buf, file.gcount());
    tmp_stream_bz_out.write(buf, file.gcount());
    tmp_stream.write(buf, file.gcount());
    if (file.eof()) break;
  }

  tmp_stream_bz_out.flush();
  tmp_stream.flush();

  out_filter.pop();
  out_filter.pop();

  tmp_stream.close();
  tmp_stream_bz.close();

  file.close();

  string hash_str = hash.get();
  n.sha256(hash_str);

  debug << "Calculated hash for " << source << " as " << hash_str;

  string a = hash_str.substr(0, 2);
  string b = hash_str.substr(2, 2);
  string rest = hash_str.substr(4);

  string final_path = _path + "/" + a + "/" + b;
  string final_name = final_path + "/" + rest;
  string final_name_bz = final_name + ".bz2";

  if (boost::filesystem::exists(final_name) ||
      boost::filesystem::exists(final_name_bz)) {
    debug << "Already have: " << hash_str;
    remove(tmp_path.c_str());
    remove(tmp_path_bz.c_str());
    return;
  }

  debug << "Storing: " << hash_str;

  string src_path;
  string dst_path;

  if (file_size(tmp_path_bz) < file_size(tmp_path)) {
    src_path = tmp_path_bz;
    dst_path = final_name_bz;
    remove(tmp_path);
  } else {
    src_path = tmp_path;
    dst_path = final_name;
    remove(tmp_path_bz);
  }

  create_directories(final_path);

  if (rename(src_path.c_str(), dst_path.c_str()) != 0) {
    throw LocalStorageException("Failed to rename file " + src_path + " to " +
                                dst_path + ": " + strerror(errno));
  }
}
}

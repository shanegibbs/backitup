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

namespace fs = boost::filesystem;

namespace backitup {

static Log LOG = Log("LocalStorage");

mutex store_mutex;

static string to_hashpath(string hash_str) {
  string a = hash_str.substr(0, 2);
  string b = hash_str.substr(2, 2);
  string rest = hash_str.substr(4);
  return a + "/" + b;
}

static string to_hashname(string hash_str) {
  string a = hash_str.substr(0, 2);
  string b = hash_str.substr(2, 2);
  string rest = hash_str.substr(4);
  return a + "/" + b + "/" + rest;
}

LocalStorage::LocalStorage(const std::string& path) : _path(path) {
  info << "Local storage path: " << path;

  if (fs::exists(path)) return;

  debug << "Creating directory " << path;
  if (mkdir(path.c_str(), 0755) == -1) {
    fatal << "Failed to mkdir for storage";
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

  string source = base_path + "/" + n.path() + "/" + n.name();

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

  string final_path = to_hashpath(hash_str);
  string final_name = _path + to_hashname(hash_str);
  string final_name_bz = final_name + ".bz2";

  if (fs::exists(final_name) || fs::exists(final_name_bz)) {
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

void LocalStorage::retrieve(const Node n, string dst) {
  string source = _path + "/" + to_hashname(n.sha256());
  string source_bz = source + ".bz2";

  Hash hash;
  int bufLen = 4096;
  char buf[bufLen];

  fs::path dst_name = dst + "/" + n.name();
  fs::create_directories(dst_name.parent_path());

  ofstream dst_stream(dst_name.string(), ios::out | ios::binary | ios::trunc);
  if (!dst_stream.is_open()) {
    error << "Failed to open: " + dst_name.string();
    return;
  }

  bool is_bz = false;

  if (fs::exists(source_bz)) {
    debug << "bz exists " << source_bz;
    is_bz = true;
  } else if (fs::exists(source)) {
    debug << "non-bz exists " << source;
    is_bz = false;
  } else {
    error << "Unable to find blob " << source << " or " << source_bz;
    return;
  }

  if (is_bz) {
    source = source_bz;
  }

  ifstream file(source, ios::in | ios::binary);
  if (!file.is_open()) {
    throw LocalStorageException("Failed to open: " + source);
  }

  filtering_streambuf<input> in_filter;
  if (is_bz) in_filter.push(bzip2_decompressor());
  in_filter.push(file);
  istream in_stream(&in_filter);

  while (!in_stream.eof()) {
    in_stream.read(buf, bufLen);
    hash.update(buf, in_stream.gcount());
    dst_stream.write(buf, in_stream.gcount());
    if (in_stream.eof()) break;
  }

  file.close();

  string hash_str = hash.get();

  dst_stream.flush();
  dst_stream.close();

  if (hash_str != n.sha256()) {
    error << "Hash does not match. Block is corrupt.";
  } else {
    debug << "Restored and hash matches " << n.sha256();
  }
}
}

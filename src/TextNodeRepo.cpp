#include "TextNodeRepo.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>

#include <boost/algorithm/string.hpp>

#include "Node.h"
#include "Record.h"

using namespace std;

namespace backitup {

Record::Record(const string &line) {
  std::vector<std::string> strs;
  boost::split(strs, line, boost::is_any_of("\t"));
  _path = strs[0];
  _name = strs[1];
  _timestamp = stol(strs[2].c_str());
  _size = stol(strs[3].c_str());
  _hash = strs[4];
}

std::string Record::to_line() const {
  stringstream ss;
  ss << _path << "\t" << _name << "\t" << _timestamp << "\t" << _size << "\t"
     << _hash;
  return ss.str();
}

void Record::dump() const {
  cout << "Record [path=" << _path << ", name=" << _name
       << ",timestamp=" << _timestamp << ", size=" << _size
       << ", hash=" << _hash << "]" << endl;
}

TextNodeRepo::TextNodeRepo() {
  cout << "Loading scratch.txt.db" << endl;
  ifstream in("scratch.txt.db");

  unsigned int count = 0;

  string line;
  while (getline(in, line)) {
    Record rec(line);
    rec.dump();

    records[rec.path()][rec.name()].push_back(rec);
    cout << count << " Loaded " << rec.name() << endl;
    count++;
  }

  in.close();

  cout << "Loaded " << count << " records" << endl;
}

void TextNodeRepo::dump() {
  std::ofstream out("scratch.txt.db");

  for (auto &p : records) {
    for (auto &d : p.second) {
      for (auto &r : d.second) {
        out << r.to_line() << endl;
      }
    }
  }
  /*
  {
    stringstream ss;
    for (string &s : records) {
      ss << s << endl;
    }
    // cout << ss.str();
    out << ss.str();
  }
  */

  out.close();
}

bool TextNodeRepo::contains(const Node &n) {
  // Called when we want to check a file needs backing up: have no hash at this
  // point
  // Also after sent to storage: DO have hash at this point

  // TODO this is too crude. Need to check only latest version of file.
  string path = n.path();
  if (path.empty()) {
    path = "/";
  }
  auto names = records[path];
  if (names.size() == 0) {
    cout << "TextNodeRepo does not contain path " << n.path() << endl;
  }

  auto recs = names[n.getName()];
  if (recs.size() == 0) {
    cout << "TextNodeRepo does not contain " << n.getName() << " in "
         << n.path() << endl;
  }

  for (Record &rec : recs) {
    // size mismatch
    if (n.size() != rec.size()) {
      continue;
    }

    if (n.mtime() != rec.timestamp()) {
      continue;
    }

    if (n.sha256().empty()) {
      return true;
    } else if (n.sha256() == rec.hash()) {
      return true;
    } else if (n.sha256() != rec.hash()) {
      continue;
    }
  }

  return false;
}

void TextNodeRepo::save(const Node &n) {
  stringstream ss;

  if (n.sha256().empty()) {
    cout << "Hash is empty" << endl;
    exit(1);
  }

  if (contains(n)) return;

  Record rec;
  string path = n.path();
  if (path.empty()) {
    path = "/";
  }

  rec.path(path);
  rec.name(n.getName());
  rec.hash(n.sha256());
  rec.timestamp(n.mtime());
  rec.size(n.size());

  auto &names = records[path];
  auto &recs = names[n.getName()];
  recs.push_back(rec);
}

NodeListRef TextNodeRepo::latestListOfPath(const string &path) {
  // cout << "TextNodeRepo generating latest NodeList of " << path << endl;
  auto list = NodeList::New(path);

  auto &names = records[path];  // list of files in path
  for (auto &d : names) {       // for each file
    Record *latest = nullptr;
    for (auto &r : d.second) {  // for each version
      if (latest == nullptr || r.timestamp() > latest->timestamp()) {
        latest = &r;
      }
    }
    Node n = Node(0, latest->name(), nullptr);
    n.path(latest->path());
    n.mtime(latest->timestamp());
    n.size(latest->size());
    n.sha256(latest->hash());
    // n.dump();
    list->add(n);
  }

  return list;
}
}
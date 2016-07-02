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
    // rec.dump();

    records[rec.path()][rec.name()].push_back(rec);
    // cout << count << " Loaded " << rec.name() << endl;
    count++;
  }

  in.close();

  // cout << "Loaded " << count << " records" << endl;
}

void TextNodeRepo::flush() {
  /*
  std::ofstream out("scratch.txt.db");
  for (auto &p : records) {
    for (auto &d : p.second) {
      for (auto &r : d.second) {
        out << r.to_line() << endl;
      }
    }
  }
  out.close();
   */
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
    // cout << "TextNodeRepo does not contain path " << n.path() << endl;
  }

  auto recs = names[n.getName()];
  if (recs.size() == 0) {
    // cout << "TextNodeRepo does not contain " << n.getName() << " in "
    //      << n.path() << endl;
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
  // cout << "TextNodeRepo Saving ";
  // n.dump();

  stringstream ss;

  if (n.sha256().empty()) {
    throw TextNodeRepoSaveException("Field sha256 missing");
  }

  if (contains(n)) {
    // cout << "TextNodeRepo Not saving. Already contains ";
    // n.dump();
    return;
  }

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

  ofstream outfile;
  outfile.open("scratch.txt.db", ios_base::app);
  outfile << rec.to_line() << endl;
}

void TextNodeRepo::deleted(const Node &n, long mtime) {
  Node a = n;
  a.sha256(string("_"));
  a.mtime(mtime);
  save(a);
}

NodeList TextNodeRepo::latest(const string &path) {
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
    if (latest->hash() == "_") {
      continue;
    }
    Node n = Node(0, latest->name(), nullptr);
    n.path(latest->path());
    n.mtime(latest->timestamp());
    n.size(latest->size());
    n.sha256(latest->hash());
    // n.dump();
    list->add(n);
  }

  return *list;
}

string TextNodeRepo::dump() const {
  vector<string> strs;
  for (auto &p : records) {
    for (auto &d : p.second) {
      for (auto &r : d.second) {
        stringstream line;
        line << r.path() << " " << r.name() << " " << r.size() << " "
             << r.hash();
        strs.push_back(line.str());
      }
    }
  }

  sort(strs.begin(), strs.end());

  stringstream ss;
  for (auto &s : strs) {
    ss << s << endl;
  }
  return ss.str();
}
}
#include "TextNodeRepo.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>

#include <boost/algorithm/string.hpp>

#include "Log.h"
#include "Node.h"
#include "Record.h"

using namespace std;

namespace backitup {

static Log LOG = Log("TextNodeRepo");

Record::Record(const string &line) {
  std::vector<std::string> strs;
  boost::split(strs, line, boost::is_any_of("\t"));
  _type = strs[0].front();
  _path = strs[1];
  _name = strs[2];
  _timestamp = stol(strs[3].c_str());
  _size = stol(strs[4].c_str());
  _hash = strs[5];
}

std::string Record::to_line() const {
  stringstream ss;
  ss << _type << "\t" << _path << "\t" << _name << "\t" << _timestamp << "\t"
     << _size << "\t" << _hash;
  return ss.str();
}

string Record::dump() const {
  stringstream ss;
  ss << "Record [type=" << _type << ", path=" << _path << ", name=" << _name
     << ",timestamp=" << _timestamp << ", size=" << _size << ", hash=" << _hash
     << "]";
  return ss.str();
}

TextNodeRepo::TextNodeRepo(string index_path) {
  _index_path = index_path;

  info << "Loading " << _index_path;
  ifstream in(_index_path);

  unsigned int count = 0;

  string line;
  while (getline(in, line)) {
    Record rec(line);
    // debug << "rec.dump() " << rec.dump();

    auto &recs = records[rec.path()][rec.name()];
    recs.push_back(rec);
    sort(recs.begin(), recs.end(), less<Record>());
    // cout << count << " Loaded " << rec.name() << endl;

    count++;
  }

  in.close();

  info << "Loaded " << count << " records";
}

void TextNodeRepo::flush() {}

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

  auto recs = names[n.name()];
  if (recs.size() == 0) {
    // cout << "TextNodeRepo does not contain " << n.name() << " in "
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

  if (n.sha256().empty() && !n.is_dir()) {
    throw TextNodeRepoSaveException("Field sha256 missing");
  }

  if (contains(n)) {
    // cout << "TextNodeRepo Not saving. Already contains ";
    // n.dump();
    return;
  }

  Record rec;
  string path = n.path();

  if (n.is_dir()) {
    rec.type('D');
  } else {
    rec.type('F');
  }

  rec.path(path);
  rec.name(n.name());
  rec.hash(n.sha256());
  rec.timestamp(n.mtime());
  rec.size(n.size());

  auto &names = records[path];
  auto &recs = names[n.name()];
  recs.push_back(rec);
  sort(recs.begin(), recs.end(), less<Record>());

  ofstream outfile;
  outfile.open(_index_path, ios_base::app);
  outfile << rec.to_line() << endl;
}

void TextNodeRepo::deleted(const Node &n, long mtime) {
  Node a = n;
  a.sha256(string("_"));
  a.mtime(mtime);
  save(a);
}

NodeList TextNodeRepo::list(string path, time_t ts) {
  auto list = NodeList::New(path);

  auto &names = records[path];  // list of files in path
  for (auto &d : names) {       // for each file
    Record *latest = nullptr;
    for (auto &r : d.second) {  // for each version
      if (r.timestamp() <= ts) {
        latest = &r;
      } else {
        break;
      }
    }
    if (latest == nullptr || latest->hash() == "_") {
      continue;
    }
    Node n = Node();
    n.name(latest->name());
    n.path(latest->path());
    n.mtime(latest->timestamp());
    n.size(latest->size());
    n.sha256(latest->hash());
    n.is_dir(latest->type() == 'D');
    // n.dump();
    list->add(n);
  }

  return *list;
}

string TextNodeRepo::dump() {
  // sorted list of paths
  vector<string> paths;
  for (auto &p : records) {
    paths.push_back(p.first);
  }
  sort(paths.begin(), paths.end());

  vector<string> strs;
  for (auto path : paths) {
    auto p = records[path];
    vector<string> names;
    for (auto &name : p) {
      names.push_back(name.first);
    }
    sort(names.begin(), names.end());

    for (auto &name : names) {
      auto &records = p[name];
      for (auto &r : records) {
        stringstream line;
        line << "." << r.path() << " " << r.name() << " " << r.size() << " "
             << (r.hash().empty() ? "x" : r.hash());
        strs.push_back(line.str());
      }
    }
  }

  stringstream ss;
  for (auto &s : strs) {
    ss << s << endl;
  }
  return ss.str();
}
}
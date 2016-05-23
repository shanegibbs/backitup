#include "TextNodeRepo.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <functional>
#include <sstream>
#include <fstream>
#include <string>

#include <boost/algorithm/string.hpp>

#include "Node.h"

using namespace std;

namespace backitup {

Record::Record(const string &line) {
  std::vector<std::string> strs;
  boost::split(strs, line, boost::is_any_of("\t"));
  _name = strs[0];
  _timestamp = stol(strs[1].c_str());
  _size = stol(strs[2].c_str());
  _hash = strs[3];
}

std::string Record::to_line() const {
  stringstream ss;
  ss << _name << "\t" << _timestamp << "\t" << _size << "\t" << _hash;
  return ss.str();
}

TextNodeRepo::TextNodeRepo() {
  cout << "Loading scratch.txt.db" << endl;
  ifstream in("scratch.txt.db");

  string line;
  while (getline(in, line)) {
    // cout << "Loaded " << line << endl;
    // records.push_back(line);
  }

  in.close();

  cout << "Loaded " << records.size() << " records" << endl;
}

void TextNodeRepo::dump() {
  std::ofstream out("scratch.txt.db");

  for (auto &p : records) {
    for (auto &r : p.second) {
      out << r.to_line() << endl;
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

bool TextNodeRepo::contains(Node &n) {
  // TODO this is too crude. Need to check only latest version of file.
  auto recs = records[n.getFullPath()];
  for (Record &rec : recs) {
    if ((n.sha256().empty() || n.sha256() == rec.hash()) && n.size() == rec.size() && n.mtime() == rec.timestamp()) {
      return true;
    }
  }

  /*
  for (string &r : records) {
    Record rec = r;

    if (n.getFullPath() == rec.name()) {
      if (n.mtime() == rec.timestamp() && n.size() == rec.size()) {
        // cout << "We already have " << n.getFullPath() << endl;
        return false;
      }
    }
  }
  */

  return false;
}

void TextNodeRepo::save(Node &n) {
  stringstream ss;

  if (n.sha256().empty()) {
    cout << "Hash is empty" << endl;
    exit(1);
  }

  if (contains(n)) return;

  Record rec;
  rec.name(n.getFullPath());
  rec.hash(n.sha256());
  rec.timestamp(n.mtime());
  rec.size(n.size());

  auto &recs = records[n.getFullPath()];
  recs.push_back(rec);

  /*
  for (string &r : records) {
    Record rec = r;

    if (n.getFullPath() == rec.name() && n.sha256() == rec.hash() &&
        n.mtime() == rec.timestamp()) {
      cout << "Skipping, already stored in db." << endl;
      return;
    }
  }

  ss << n.getFullPath() << "\t" << n.mtime() << "\t" << n.size() << "\t"
     << n.sha256();

  records.push_back(ss.str());
  */
}
}

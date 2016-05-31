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
#include "Record.h"

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

  unsigned int count = 0;

  string line;
  while (getline(in, line)) {
    Record rec(line);

    records[rec.name()].push_back(rec);
    // cout << "Loaded " << line << endl;
    count++;
  }

  in.close();

  cout << "Loaded " << count << " records" << endl;
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
  // Called when we want to check a file needs backing up: have no hash at this
  // point
  // Also after sent to storage: DO have hash at this point

  // TODO this is too crude. Need to check only latest version of file.
  auto recs = records[n.getFullPath()];
  if (recs.size() == 0) {
    cout << "No records found for " << n.getFullPath() << endl;
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

void TextNodeRepo::save(RecordSet &r) {
  cout << "Saving RecordSet" << endl;
}
}

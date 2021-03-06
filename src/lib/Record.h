#ifndef RECORD_H_
#define RECORD_H_

#include <map>
#include <string>
#include <vector>

namespace backitup {

class Node;

class Record {
 public:
  Record() {}
  Record(const std::string &line);
  std::string to_line() const;
  char type() const { return _type; }
  void type(char t) { _type = t; }
  std::string &path() const { return const_cast<std::string &>(_path); }
  void path(std::string p) { _path = p; }
  std::string &name() const { return const_cast<std::string &>(_name); }
  void name(std::string n) { _name = n; }
  long timestamp() const { return _timestamp; }
  void timestamp(long t) { _timestamp = t; }
  long size() const { return _size; }
  void size(long s) { _size = s; }
  std::string &hash() const { return const_cast<std::string &>(_hash); }
  void hash(std::string h) { _hash = h; }
  std::string dump() const;

  bool operator>(const Record &n) const { return (_timestamp > n._timestamp); }
  bool operator<(const Record &n) const { return (_timestamp < n._timestamp); }

 private:
  char _type;
  std::string _path;
  std::string _name;
  long _timestamp;
  long _size;
  std::string _hash;
};

typedef std::shared_ptr<std::vector<Record>> RecordList;

RecordList NewRecordList();

class RecordSet {
 public:
  RecordSet() {}
  std::string path() const { return _path; }
  void path(std::string s) { _path = s; }

  void addRecord(Record r) { _records.push_back(r); }

 private:
  std::string _path;
  std::vector<Record> _records;
};
}
#endif

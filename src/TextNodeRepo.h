#ifndef TEXT_NODE_REPO_H_
#define TEXT_NODE_REPO_H_

#include <vector>
#include <string>
#include <map>

namespace backitup {

class Node;

class Record {
 public:
  Record() {}
  Record(const std::string &line);
  std::string to_line() const;
  std::string &name() { return _name; }
  void name(std::string n) { _name = n; }
  long timestamp() const { return _timestamp; }
  void timestamp(long t) { _timestamp = t; }
  long size() const { return _size; }
  void size(long s) { _size = s; }
  std::string &hash() { return _hash; }
  void hash(std::string h) { _hash = h; }

 private:
  std::string _name;
  long _timestamp;
  long _size;
  std::string _hash;
};

class TextNodeRepo {
 public:
  TextNodeRepo();
  bool contains(Node &n);
  void save(Node &n);
  void compact() {}

  const Node& getParent(Node &n);

  void dump();

 private:
  std::map<std::string, std::vector<Record>> records;
};
}

#endif

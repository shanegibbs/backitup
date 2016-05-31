#ifndef TEXT_NODE_REPO_H_
#define TEXT_NODE_REPO_H_

#include <vector>
#include <string>
#include <map>

#include "Record.h"

namespace backitup {

class Node;

class TextNodeRepo {
 public:
  TextNodeRepo();
  bool contains(Node &n);
  void save(Node &n);
  void save(RecordSet &r);
  void compact() {}

  const Node &getParent(Node &n);

  void dump();

 private:
  std::map<std::string, std::vector<Record>> records;
};
}

#endif

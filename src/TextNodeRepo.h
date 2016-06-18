#ifndef TEXT_NODE_REPO_H_
#define TEXT_NODE_REPO_H_

#include <map>
#include <string>
#include <vector>

#include "Node.h"
#include "Record.h"

namespace backitup {

class Node;

class TextNodeRepo {
 public:
  TextNodeRepo();
  bool contains(const Node &n);
  void save(const Node &n);
  void deleted(const Node &n, long mtime);
  void compact() {}

  NodeListRef latestListOfPath(const string &path);
  NodeListRef diff(NodeListRef r);

  const Node &getParent(const Node &n);

  void dump();

 private:
  std::map<std::string, std::map<std::string, std::vector<Record>>> records;
};
}

#endif

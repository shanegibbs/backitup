#ifndef TEXT_NODE_REPO_H_
#define TEXT_NODE_REPO_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "Index.h"
#include "Node.h"
#include "Record.h"

namespace backitup {

class Node;

class TextNodeRepoSaveException : public std::runtime_error {
 public:
  TextNodeRepoSaveException() : runtime_error("Unable to save node") {}
  TextNodeRepoSaveException(std::string msg)
      : runtime_error(
            string(string("Unable to save node: ").append(msg)).c_str()) {}
};

class TextNodeRepo : public Index {
 public:
  TextNodeRepo(std::string index_path);
  bool contains(const Node &n);
  void save(const Node &n);
  void deleted(const Node &n, long mtime);
  void compact() {}

  NodeList list(string path, time_t ts);
  NodeList list(string path) { return list(path, time(nullptr)); }

  NodeListRef diff(NodeListRef r);

  const Node &getParent(const Node &n);

  void flush();

  enum ReturnCodes { FailedToSave };

  string dump();

 private:
  std::string _index_path;
  std::unordered_map<std::string,
                     std::unordered_map<std::string, std::vector<Record>>>
      records;
};
}

#endif

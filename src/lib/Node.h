/*
 * Node.h
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#ifndef NODE_H_
#define NODE_H_

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

namespace backitup {

class Node {
 public:
  Node(unsigned int id, const string name, shared_ptr<Node> parent);
  Node(const string path, const string name, long mtime, long size,
       string sha256);

  const string getFullPath() const;

  const unsigned int getId() const { return id; }

  void setId(unsigned int id) { this->id = id; }

  const string& getName() const { return name; }

  const string& path() const { return _path; }

  void path(const string& p) { _path = p; }

  shared_ptr<Node> getParent() const { return parent; }

  long mtime() const { return _mtime; }

  void mtime(long t) { _mtime = t; }

  long size() const { return _size; }

  void size(long s) { _size = s; }

  const string sha256() const { return _sha256; }

  void sha256(const string& s) { _sha256 = s; }

  void addChild(shared_ptr<Node> n) { children_.push_back(n); }

  const vector<shared_ptr<Node>>& children() const { return children_; }

  static shared_ptr<Node> create(int id, const string name,
                                 shared_ptr<Node> parent) {
    return shared_ptr<Node>(new Node(id, name, parent));
  }

  static shared_ptr<Node> createRoot() {
    return shared_ptr<Node>(new Node(0, "", shared_ptr<Node>()));
  }

  bool operator>(const Node& n) const { return (getName() > n.getName()); }
  bool operator<(const Node& n) const { return (getName() < n.getName()); }

  void dump() const {
    cout << "Node [path=" << _path << ", name=" << name << ", mtime=" << _mtime
         << ", size=" << _size << ", sha256=" << _sha256 << "]" << endl;
  }

 private:
  unsigned int id;
  string _path;
  string name;
  long _size;
  long _mtime;
  string _sha256;
  shared_ptr<Node> parent;
  vector<shared_ptr<Node>> children_;
};

class NodeList;
typedef std::shared_ptr<NodeList> NodeListRef;

class NodeList {
 public:
  NodeList(const std::string& p) { _path = p; }
  static NodeListRef New(const std::string& p) {
    return NodeListRef(new NodeList(p));
  }
  const std::vector<Node>& list() const { return _list; }
  void add(Node& n) { _list.push_back(n); }

  const string& path() const { return _path; }

  void mtime(long m) { _mtime = m; }
  long mtime() const { return _mtime; }

  string dump() const;

 private:
  std::string _path;
  long _mtime;
  std::vector<Node> _list;
};
}

#endif /* NODE_H_ */

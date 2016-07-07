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
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace backitup {

class Node {
 public:
  Node() {
    _size = 0;
    _mtime = 0;
  }
  Node(const string path, const string name, bool is_dir = false)
      : _path(path), _name(name), _is_dir(is_dir) {
    _size = 0;
    _mtime = 0;
  }
  Node(const string path, const string name, long mtime, long size,
       string sha256);

  const string full_path() const;

  void name(const string& n) { _name = n; }

  const string& name() const { return _name; }

  bool is_dir() const { return _is_dir; }

  const string& path() const { return _path; }

  void path(const string& p) { _path = p; }

  long mtime() const { return _mtime; }

  void mtime(long t) { _mtime = t; }

  long size() const { return _size; }

  void size(long s) { _size = s; }

  const string sha256() const { return _sha256; }

  void sha256(const string& s) { _sha256 = s; }

  bool operator>(const Node& n) const { return (_name > n._name); }
  bool operator<(const Node& n) const { return (_name < n._name); }

  std::string dump() const {
    std::stringstream ss;
    ss << "Node [path=" << _path << ", name=" << _name << ", mtime=" << _mtime;
    if (is_dir()) {
      ss << ", dir]";
    } else {
      ss << ", size=" << _size << ", sha256=" << _sha256 << "]";
    }
    return ss.str();
  }

 private:
  string _path;
  string _name;
  bool _is_dir = false;
  long _size;
  long _mtime;
  string _sha256;
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

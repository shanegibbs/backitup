/*
 * Node.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <sstream>
#include <stack>

#include "Node.h"

namespace backitup {

Node::Node(unsigned int id, const string name, shared_ptr<Node> parent)
    : id(id), name(name), parent(parent) {}

Node::Node(const string path, const string name, long mtime, long size,
           string sha256)
    : _path(path), name(name), _size(size), _mtime(mtime), _sha256(sha256) {}

const string Node::getFullPath() const {
  std::stringstream ss;

  if (!_path.empty()) {
    ss << _path << "/" << name;
  } else {
    stack<const Node *> chain;

    const Node *n = this;
    while (n) {
      chain.push(n);
      n = n->parent.get();
    }

    while (!chain.empty()) {
      n = chain.top();
      if (!n->name.empty()) ss << "/";
      ss << n->name;
      chain.pop();
    }
  }
  return ss.str();
}
}

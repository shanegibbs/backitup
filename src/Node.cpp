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

const string Node::getFullPath() const {
  std::stringstream ss;

  if (!_path.empty()) {
    ss << _path << name;
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

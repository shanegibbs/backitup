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

Node::Node(const unsigned int id, const string name, shared_ptr<const Node> parent) : id(id), name(name), parent(parent) {

}

const shared_ptr<const string> Node::getFullPath() const {

  stack<const Node*> chain;

  const Node *n = this;
  while (n) {
    chain.push(n);
    n = n->parent.get();
  }

  std::stringstream ss;
  while (!chain.empty()) {
    n = chain.top();
    if (!n->name.empty()) ss << "/";
    ss << n->name;
    chain.pop();
  }

  return shared_ptr<const string>(new string(ss.str()));
}

}

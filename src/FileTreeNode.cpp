/*
 * FileTreeNode.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <sstream>
#include <stack>

#include "FileTreeNode.h"

namespace backitup {

FileTreeNode::FileTreeNode(int id, const string name, shared_ptr<const FileTreeNode> parent) : name(name), parent(parent) {

}

const shared_ptr<const string> FileTreeNode::getFullPath() const {

  stack<const FileTreeNode*> chain;

  const FileTreeNode *n = this;
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

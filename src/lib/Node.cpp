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

Node::Node(const string path, const string name, long mtime, long size,
           string sha256)
    : _path(path), _name(name), _size(size), _mtime(mtime), _sha256(sha256) {}

const string Node::full_path() const {
  std::stringstream ss;
  if (!_path.empty()) ss << _path << "/";
  ss << _name;
  return ss.str();
}

string NodeList::dump() const {
  stringstream ss;
  ss << _path << endl;
  for (auto& n : _list) {
    ss << n.dump() << endl;
  }
  return ss.str();
}
}

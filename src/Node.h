/*
 * Node.h
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#ifndef NODE_H_
#define NODE_H_

#include <string>
#include <memory>
#include <vector>

using namespace std;

namespace backitup {

class Node {
 public:
  Node(unsigned int id, const string name, shared_ptr<Node> parent);

  const string getFullPath() const;

  const unsigned int getId() const { return id; }

  void setId(unsigned int id) { this->id = id; }

  const string& getName() const { return name; }

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

 private:
  unsigned int id;
  const string name;
  long _size;
  long _mtime;
  string _sha256;
  shared_ptr<Node> parent;
  vector<shared_ptr<Node>> children_;
};
}

#endif /* NODE_H_ */

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

using namespace std;

namespace backitup {

class Node {
public:
  Node(const unsigned int id, const string name, shared_ptr<const Node> parent);

  const shared_ptr<const string> getFullPath() const;

  const unsigned int getId() const {
    return id;
  }

  void setId(unsigned int id) {
    this->id = id;
  }

  const string &getName() const {
    return name;
  }

  shared_ptr<const Node> getParent() const {
    return parent;
  }

  static shared_ptr<const Node> create(int id, const string name, shared_ptr<const Node> parent) {
    return shared_ptr<const Node>(new Node(id, name, parent));
  }

  static shared_ptr<Node> createRoot() {
    return shared_ptr<Node>(new Node(0, "", shared_ptr<Node>()));
  }

private:
  unsigned int id;
  const string name;
  shared_ptr<const Node> parent;
};
}

#endif /* NODE_H_ */

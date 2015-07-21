/*
 * FileTreeNode.h
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#ifndef FILETREENODE_H_
#define FILETREENODE_H_

#include <string>
#include <memory>

using namespace std;

namespace backitup {

class FileTreeNode {
public:
  FileTreeNode(int id, const string name, shared_ptr<const FileTreeNode> parent);

  const shared_ptr<const string> getFullPath() const;

  static shared_ptr<const FileTreeNode> create(int id, const string name, shared_ptr<const FileTreeNode> parent) {
    return shared_ptr<const FileTreeNode>(new FileTreeNode(id, name, parent));
  }

  static shared_ptr<const FileTreeNode> createRoot() {
    return shared_ptr<const FileTreeNode>(new FileTreeNode(0, "", shared_ptr<const FileTreeNode>()));
  }

private:
  const string name;
  shared_ptr<const FileTreeNode> parent;
};
}

#endif /* FILETREENODE_H_ */

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

private:
  const string name;
  shared_ptr<const FileTreeNode> parent;
};
}

#endif /* FILETREENODE_H_ */

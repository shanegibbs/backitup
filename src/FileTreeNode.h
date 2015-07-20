/*
 * FileTreeNode.h
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#ifndef FILETREENODE_H_
#define FILETREENODE_H_

#include <string>

using namespace std;

namespace backitup {

class FileTreeNode {
public:
  FileTreeNode(int id, const string name, const FileTreeNode *parent = nullptr);

  const shared_ptr<const string> getFullPath() const;

private:
  const string name;
  const FileTreeNode *parent;
};
}

#endif /* FILETREENODE_H_ */

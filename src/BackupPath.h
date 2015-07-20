/*
 * BackupPath.h
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#ifndef BACKUPPATH_H_
#define BACKUPPATH_H_

#include <string>
#include <vector>

using namespace std;

namespace backitup {

class FileTreeNode;

class BackupPath {
public:
  BackupPath(const string name, const string path);

  static shared_ptr<BackupPath> create(const string name, const string path) {
    return shared_ptr<BackupPath>(new BackupPath(name, path));
  }

  shared_ptr<vector<const FileTreeNode*>> listFiles();

  void visitFiles(void (*)(FileTreeNode*));

private:
  const string name;
  const string path;
};
}

#endif /* BACKUPPATH_H_ */

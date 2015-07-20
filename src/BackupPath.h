/*
 * BackupPath.h
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#ifndef BACKUPPATH_H_
#define BACKUPPATH_H_

#include <functional>
#include <memory>
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

  void visitFiles(function<void(shared_ptr<const FileTreeNode>)> fn) const;

private:
  const string name;
  const string path;
};
}

#endif /* BACKUPPATH_H_ */

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

#include "Node.h"

using namespace std;

namespace backitup {

class Node;
class FileIndex;

class BackupPath {
 public:
  BackupPath(const string path);

  static shared_ptr<BackupPath> create(const string path) {
    return shared_ptr<BackupPath>(new BackupPath(path));
  }

  shared_ptr<Node> visitFiles(function<void(shared_ptr<Node>)> fn) const;

  void watchFiles(function<void(NodeListRef)> fn) const;

 private:
  const string path;
};
}

#endif /* BACKUPPATH_H_ */

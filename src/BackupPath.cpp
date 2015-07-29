/*
 * BackupPath.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include <functional>

#include "BackupPath.h"
#include "Node.h"

namespace backitup {

BackupPath::BackupPath(const std::string p) : path(p) {

}

void visitFilesRecursive(const string &base, shared_ptr<Node> node, function<void(shared_ptr<Node>)> fn) {
  DIR *d;
  struct dirent *dir;
  string fullPath = base + *node->getFullPath();
  d = opendir(fullPath.c_str());
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (strcmp(dir->d_name, ".") == 0) continue;
      if (strcmp(dir->d_name, "..") == 0) continue;
      shared_ptr<Node> child(new Node(0, dir->d_name, node));

      if (dir->d_type == DT_DIR) {
        fn(child);
        visitFilesRecursive(base, child, fn);
      } else if (dir->d_type == DT_REG) {
        fn(child);
      }
    }
    closedir(d);
  }
}

void BackupPath::visitFiles(function<void(shared_ptr<Node>)> fn) const {
  visitFilesRecursive(path, Node::createRoot(), fn);
}

}

/*
 * BackupPath.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <dirent.h>
#include <stdio.h>

#include "BackupPath.h"

#include "FileTreeNode.h"

namespace backitup {

BackupPath::BackupPath(const std::string n, const std::string p) : name(n), path(p) {

}

void addDirectoryListing(const string &base, const FileTreeNode &node, vector<const FileTreeNode*> &list, void (*fn)(FileTreeNode*)) {
  DIR *d;
  struct dirent *dir;
  string fullPath = base + *node.getFullPath();
  d = opendir(fullPath.c_str());
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (strcmp(dir->d_name, ".") == 0) continue;
      if (strcmp(dir->d_name, "..") == 0) continue;
      const FileTreeNode *child = new FileTreeNode(0, dir->d_name, &node);

      if (dir->d_type == DT_DIR) {
        addDirectoryListing(base, *child, list, fn);
      } else if (dir->d_type == DT_REG) {
        list.push_back(child);
      }
    }
    closedir(d);
  }
}

shared_ptr<vector<const FileTreeNode*>> BackupPath::listFiles() {
  shared_ptr<vector<const FileTreeNode*>> files(new vector<const FileTreeNode*>());

  const FileTreeNode *n = new FileTreeNode(0, "", nullptr);
  addDirectoryListing(path, *n, *files.get(), 0);

  return files;
}

void BackupPath::visitFiles(void (*fn)(FileTreeNode*)) {
  // const FileTreeNode *n = new FileTreeNode(0, "", nullptr);
  // addDirectoryListing(path, *n, nullptr, fn);
}

}

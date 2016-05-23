/*
 * BackupPath.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <iostream>
#include <functional>
#include <ctime>

#include <boost/filesystem/operations.hpp>

#include "BackupPath.h"
#include "Node.h"

namespace backitup {

BackupPath::BackupPath(const std::string p) : path(p) {}

/*
static string ts_to_string(long ts) {
  std::time_t now = (const time_t)ts;
  std::tm *ptm = std::localtime(&now);
  char buffer[32];
  std::strftime(buffer, 32, "%a, %Y-%m-%d %H:%M:%S", ptm);
  return string(buffer);
}
*/

void visitFilesRecursive(const string &base, shared_ptr<Node> node,
                         function<void(shared_ptr<Node>)> fn) {
  DIR *d;
  struct dirent *dir;
  string fullPath = base + node->getFullPath();
  d = opendir(fullPath.c_str());
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (strcmp(dir->d_name, ".") == 0) continue;
      if (strcmp(dir->d_name, "..") == 0) continue;
      shared_ptr<Node> child(new Node(0, dir->d_name, node));
      node->addChild(child);

      if (dir->d_type == DT_DIR) {
        child->mtime(0);
        child->size(0);
        fn(child);
        visitFilesRecursive(base, child, fn);

      } else if (dir->d_type == DT_REG) {
        string filename = base + child->getFullPath();
        struct stat s;
        if (stat(filename.c_str(), &s) == -1) {
          cout << "Failed to read " << filename << endl;
        } else {
#if __APPLE__ && __MACH__
          long mtime = s.st_mtime;
          long ctime = s.st_ctime;

          if (ctime > mtime) mtime = ctime;

          child->mtime(mtime);
          child->size(s.st_size);
#else
#error TODO Linux support for mtime
#endif
        }

        fn(child);
      }
    }
    closedir(d);
  }
}

shared_ptr<Node> BackupPath::visitFiles(
    function<void(shared_ptr<Node>)> fn) const {
  shared_ptr<Node> root = Node::createRoot();
  visitFilesRecursive(path, Node::createRoot(), fn);
  return root;
}
}

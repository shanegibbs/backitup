/*
 * main.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <functional>

#include DB_CXX_HEADER

#include "BackupPath.h"
#include "FileTreeNode.h"
#include "FileIndex.h"

using namespace std;
using namespace backitup;

int main(int argc, char** argv) {

  auto index = FileIndex::create("snowy-lang");

  auto b = BackupPath::create(index, "/home/sgibbs/Documents");

  unsigned int fileCount = 0;
  b->visitFiles([&] (shared_ptr<const FileTreeNode> f) -> void {
    fileCount++;
    // printf("%s\n", f->getFullPath()->c_str());
  });

  // first pass, find new, updated and deleted files
  // new files go into initial state, include metadata but no hash
  // updated files, create new version with updated metadata, but no hash

  // second pass, upload to pool, update hash

  printf("Found %d files\n", fileCount);

  return 0;
}

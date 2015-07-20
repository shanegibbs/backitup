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

#include "BackupPath.h"
#include "FileTreeNode.h"

using namespace std;
using namespace backitup;

int main(int argc, char** argv) {

  auto b = BackupPath::create("snowy-lang", "/home/sgibbs/shanegibbs/snowy-lang");

  unsigned int fileCount = 0;
  b->visitFiles([&] (shared_ptr<const FileTreeNode> f) -> void {
    fileCount++;
    printf("%s\n", f->getFullPath()->c_str());
  });

  printf("Found %d files\n", fileCount);

  return 0;
}

/*
 * main.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "BackupPath.h"
#include "FileTreeNode.h"

using namespace std;
using namespace backitup;

int main(int argc, char** argv) {

  auto b = BackupPath::create("snowy-lang", "/Users/sgibbs/shanegibbs/snowy-lang");
  auto files = b->listFiles();
  for (auto f : *files) {
    printf("%s\n", f->getFullPath()->c_str());
  }
  printf("Found %lu files\n", files->size());

  return 0;
}

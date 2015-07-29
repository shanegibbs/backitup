/*
 * main.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <functional>

#include DB_CXX_HEADER

#include "BackupPath.h"
#include "Node.h"
#include "FileIndex.h"
#include "NodeRepo.h"
#include "Database.h"

using namespace std;
using namespace backitup;

int main(int argc, char** argv) {
  auto db = Database::open("index.db");
  // auto index = FileIndex::create("snowy-lang");
  NodeRepo repo(db);

  string path = argv[1];
  cout << "Path: " << path << endl;
  auto b = BackupPath::create(path);
  // auto b = BackupPath::create("/home/sgibbs/Documents");
  // auto b = BackupPath::create("/Users/sgibbs/Downloads");

  unsigned int fileCount = 0;
  b->visitFiles([&](shared_ptr<Node> f) -> void {
    // printf("%s\n", f->getFullPath()->c_str());
    fileCount++;

    try {
      repo.save(*f);
    } catch (ExistsDatabaseException e) {
      cout << "Exists" << endl;
    }

    cout << f->getId() << " " << f->getName() << endl;

  });

  // first pass, find new, updated and deleted files
  // new files go into initial state, include metadata but no hash
  // updated files, create new version with updated metadata, but no hash

  // second pass, upload to pool, update hash

  printf("Found %d files\n", fileCount);

  return 0;
}

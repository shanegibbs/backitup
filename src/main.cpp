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

#include "Log.h"
#include "BackupPath.h"
#include "Node.h"
#include "FileIndex.h"
#include "NodeRepo.h"
#include "RepositoryIndex.h"
#include "Database.h"

#include "TextNodeRepo.h"
#include "LocalStorage.h"

using namespace std;
using namespace backitup;

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << "Not path" << endl;
    return 1;
  }

  string backupName = "scratch";

  TextNodeRepo repo;

  string path = argv[1];
  cout << "Backup Path: " << path << endl;
  auto b = BackupPath::create(path);

  auto storage = LocalStorage::create("storage");

  b->watchFiles([&](shared_ptr<RecordSet> rs) -> void {
      cout << "Trigger on " << rs->path() << endl;
      repo.save(*rs);
  });

  unsigned int fileCount = 0;
  auto root = b->visitFiles([&](shared_ptr<Node> f) -> void {
    fileCount++;

    // cout << "Visited " << f->getFullPath() << " " << f->getId() << " " <<
    // f->getName() << endl;

    if (f->size() > 1024 * 1024) return;

    if (repo.contains(*f)) {
      // cout << "Already have " << f->getFullPath() << endl;
      return;
    }

    cout << "Want to backup (no mtime,size match found in index) " << f->getFullPath() << endl;

    // order is important here
    storage->send(path, *f);
    repo.save(*f);

  });

  sleep(100000);

  // repo.save(*root);

  // first pass, find new, updated and deleted files
  // new files go into initial state, include metadata but no hash
  // updated files, create new version with updated metadata, but no hash

  // second pass, upload to pool, update hash

  printf("Found %d files\n", fileCount);

  repo.dump();
  repo.compact();

  return 0;
}

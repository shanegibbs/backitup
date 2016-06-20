/*
 * main.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <execinfo.h>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include DB_CXX_HEADER

#include <BackupPath.h>
#include <Database.h>
#include <FileIndex.h>
#include <Log.h>
#include <Node.h>
#include <NodeRepo.h>
#include <RepositoryIndex.h>

#include <LocalStorage.h>
#include <TextNodeRepo.h>

using namespace std;
using namespace backitup;

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << "No path" << endl;
    return 1;
  }

  string path = argv[1];
  cout << "Backup Path: " << path << endl;

  auto b = BackupPath::create(path);
  auto storage = LocalStorage::create("storage");
  TextNodeRepo repo;

  b->watchFiles([&](NodeListRef nl) -> void {
    // cout << "\nTrigger on " << nl->path() << endl;

    auto stored = repo.latestListOfPath(nl->path());

    // check for new nodes
    for (auto& n : nl->list()) {
      const Node* found = nullptr;
      for (auto& a : stored.list()) {
        if (a.getName() == n.getName() && a.mtime() == n.mtime() &&
            a.size() == n.size()) {
          found = &a;
        }
      }
      if (found == nullptr && !repo.contains(n)) {
        cout << "New ";
        n.dump();
        Node a = n;
        storage->send(path, a);
        repo.save(a);
      }
    }

    // check for deleted nodes
    for (auto& a : stored.list()) {
      const Node* found = nullptr;
      for (auto& n : nl->list()) {
        if (a.getName() == n.getName()) {
          found = &a;
        }
      }
      if (found == nullptr) {
        cout << "Deleted ";
        a.dump();
        repo.deleted(a, nl->mtime());
      }
    }

    repo.flush();

  });

  unsigned int fileCount = 0;
  auto root =
      b->visitFiles([&](const string& path, shared_ptr<Node> f) -> void {
        fileCount++;
        if (f->size() > 1024 * 1024) return;
        if (repo.contains(*f)) {
          return;
        }

        cout << "Want to backup (no mtime,size match found in index) "
             << f->getFullPath() << endl;

        // order is important here
        storage->send(path, *f);
        repo.save(*f);
      });

  repo.flush();

  sleep(100000);

  printf("Found %d files\n", fileCount);

  return 0;
}

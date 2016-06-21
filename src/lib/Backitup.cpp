#include "Backitup.h"

namespace backitup {

Backitup::Backitup(Index& i, Storage& s) : _index(i), _store(s) {}

void Backitup::run(BackupPath& b) {
  // start watcher

  b.watchFiles([&](const string& path, NodeListRef nl) -> void {
    // cout << "\nTrigger on " << nl->path() << endl;

    auto stored = _index.latest(nl->path());

    // check for new nodes
    for (auto& n : nl->list()) {
      const Node* found = nullptr;
      for (auto& a : stored.list()) {
        if (a.getName() == n.getName() && a.mtime() == n.mtime() &&
            a.size() == n.size()) {
          found = &a;
        }
      }
      if (found == nullptr && !_index.contains(n)) {
        cout << "New ";
        n.dump();
        Node a = n;
        _store.send(path, a);
        _index.save(a);
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
        _index.deleted(a, nl->mtime());
      }
    }

    _index.flush();

  });

  // full scan

  unsigned int fileCount = 0;
  auto root = b.visitFiles([&](const string& path, shared_ptr<Node> f) -> void {
    fileCount++;
    if (f->size() > 1024 * 1024) return;
    if (_index.contains(*f)) {
      return;
    }

    cout << "Want to backup (no mtime,size match found in index) "
         << f->getFullPath() << endl;

    // order is important here
    _store.send(path, *f);
    _index.save(*f);
  });

  _index.flush();

  sleep(100000);
}
}

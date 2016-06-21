#include "Backitup.h"

namespace backitup {

Backitup::Backitup(Index& i, Storage& s) : _index(i), _store(s) {}

void Backitup::run(BackupPath& b) {
  // start watcher

  b.watchFiles([&](const string& path, NodeListRef nl) -> void {
    // cerr << "WATCH DISABLED" << endl;
    // return;

    bool changed = false;

    auto stored = _index.latest(nl->path());

    // check for new nodes
    for (auto& n : nl->list()) {
      if (n.getName() == "scratch.txt.db") continue;

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
        changed = true;
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
        changed = true;
      }
    }

    if (changed) {
      cout << "\nTrigged on " << nl->path() << endl << endl;
      _index.flush();
    }

    sleep(1);

  });

  // full scan

  unsigned int fileCount = 0;
  auto root = b.visitFiles([&](const string& path, shared_ptr<Node> f) -> void {
    if (f->getName() == "scratch.txt.db") return;

    fileCount++;
    if (f->size() > 1024 * 1024) return;
    if (_index.contains(*f)) {
      return;
    }

    cout << "Scan new ";
    f->dump();

    // order is important here
    _store.send(path, *f);
    _index.save(*f);
  });

  _index.flush();

  sleep(100000);
}
}

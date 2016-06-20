#include "Backitup.h"

namespace backitup {

Backitup::Backitup(Index& i, Storage& s) : _index(i), _store(s) {}

void Backitup::run(BackupPath& b) {
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

#include "Backitup.h"

#include "Channel.h"
#include "Log.h"

namespace backitup {

static Log LOG = Log("Backitup");

Backitup::Backitup(Index& i, Storage& s) : _index(i), _store(s) {}

void Backitup::stop() {
  _running = false;
  _chan.close();

  unique_lock<mutex> lk(_stopped_m);
  stopped_cv.wait_for(lk, 5s, [&] { return _stopped; });
  if (!_stopped) {
    throw "Failed to stop channel";
  }

  _stopped = false;
  _chan.reset();
}

void Backitup::init(BackupPath& b) {
  // start watcher

  Channel<string>& chan_ref = _chan;
  b.watch([&chan_ref](const string& changed) -> void {
     if (chan_ref.put(changed)) {
       info << "Queued for processing " << changed;
     }
   }).detach();

  // run full scan

  info << "Watcher started. Beginning full scan";

  b.visit([&](const string& path, const NodeList& nl) -> void {
    debug << "Scanning " << nl.path();
    process_nl(path, nl);
  });

  _index.flush();

  info << "Full scan completed";
}

void Backitup::run(BackupPath& b, function<void(const string& path)> fn) {
  _running = true;
  _stopped = false;
  std::thread t([&]() {
    string changed;
    while (_running && _chan.get(changed, true)) {
      if (!_running) break;

      info << "Processing " << changed;
      auto nl = b.list(changed);
      bool was_change = process_nl(b.get_path(), nl);
      if (was_change && fn != nullptr) {
        fn(changed);
      }

      if (_chan.empty() && _sleep_on_empty) {
        info << "Nothing more to backup, sleeping for 20 seconds.";
        sleep(20);
      }
    }

    {
      unique_lock<mutex> lk(_stopped_m);
      _stopped = true;
      stopped_cv.notify_one();
    }

  });
  t.detach();
}

bool Backitup::process_nl(const string& path, const NodeList& nl) {
  unsigned int fileCount = 0;
  unsigned long fileSize = 0;
  bool changed = false;

  auto stored = _index.latest(nl.path());

  // check for new nodes
  for (auto& n : nl.list()) {
    fileCount++;
    fileSize += n.size();

    if (n.getName() == "scratch.txt.db") continue;

    const Node* found = nullptr;
    for (auto& a : stored.list()) {
      if (a.getName() == n.getName() && a.mtime() == n.mtime() &&
          a.size() == n.size()) {
        found = &a;
      }
    }
    if (found == nullptr && !_index.contains(n)) {
      if (n.size() > 10 * 1024) continue;
      info << "New " << n.path() << "/" << n.getName();
      Node a = n;

      try {
        _store.send(path, a);
      } catch (exception& e) {
        warn << e.what();
        continue;
      }

      _index.save(a);
      changed = true;
    }
  }

  // check for deleted nodes
  for (auto& a : stored.list()) {
    const Node* found = nullptr;
    for (auto& n : nl.list()) {
      if (a.getName() == n.getName()) {
        found = &a;
      }
    }
    if (found == nullptr) {
      info << "Deleted " << a.path() << "/" << a.getName();
      _index.deleted(a, nl.mtime());
      changed = true;
    }
  }

  if (changed) {
    // cout << "\nTrigged on " << nl.path() << endl << endl;
    _index.flush();
  }

  return changed;
}
}

#include <ctime>
#include <iomanip>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "Backitup.h"

#include "Channel.h"
#include "Helpers.h"
#include "Log.h"

namespace fs = boost::filesystem;

namespace backitup {

static Log LOG = Log("Backitup");

Backitup::Backitup(Index& i, Storage& s) : _index(i), _store(s) {}

void Backitup::stop() {
  _running = false;
  _chan.close();

  unique_lock<mutex> lk(_stopped_m);
  stopped_cv.wait_for(lk, 5s, [&] { return _stopped; });
  if (!_stopped) {
    fatal << "Failed to stop channel";
    throw BackitupException("Failed to stop channel");
  }
}

void Backitup::init(BackupPath& b) {
  // start watcher

  Channel<string>& chan_ref = _chan;
  b.watch([&chan_ref](const string& changed) -> void {
    if (chan_ref.put(changed)) {
      debug << "Queued for processing " << changed;
    }
  });

  // run full scan

  info << "Watcher started. Beginning full scan";

  b.visit([&](const string& path, const NodeList& nl) -> void {
    debug << "Scanning " << nl.path();
    process_nl(path, nl);
  });

  _index.flush();

  info << "Full scan completed";
}

thread Backitup::run(BackupPath& b, function<void(const string& path)> fn) {
  _running = true;
  _stopped = false;
  _chan.reset();

  std::thread t([&]() {
    string changed;

    info << "Interval seconds " << _interval.second;

    while (_running) {
      if (!_running) break;

      // only attempt sleep if have nothing to process
      while (_chan.empty() && _sleep_on_empty) {
        int next_backup =
            ((time(nullptr) / _interval.second) + 1) * _interval.second;
        int sleep_needed = next_backup - time(nullptr);

        if (sleep_needed > 0) {
          info << "Nothing to backup, sleeping for " << sleep_needed
               << " seconds";

          // sleep only guarantees a minimum of seconds.
          while (time(nullptr) < next_backup) {
            sleep(1);
          }
        }
      }

      if (!_chan.get(changed, true)) continue;

      debug << "Processing " << changed;
      auto nl = b.list(changed);
      bool was_change = process_nl(b.get_path(), nl);
      if (was_change && fn != nullptr) {
        fn(changed);
      }
    }

    {
      unique_lock<mutex> lk(_stopped_m);
      _stopped = true;
      stopped_cv.notify_one();
    }

  });
  return t;
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

    if (n.name() == "scratch.txt.db") continue;

    const Node* found = nullptr;
    for (auto& a : stored.list()) {
      if (a.name() == n.name() && a.mtime() == n.mtime() &&
          a.size() == n.size()) {
        found = &a;
      }
    }
    if (found == nullptr && !_index.contains(n)) {
      if (_max_file_size_bytes != 0 && n.size() > _max_file_size_bytes) {
        warn << "Skipping large file " << n.name() << ". Size was " << n.size()
             << " bytes";
        continue;
      }

      info << "\x1b[32m+\x1b[0m " << n.full_path() << (n.is_dir() ? "/" : "");
      Node a = n;  // copy to non-const

      if (!a.is_dir()) {
        try {
          _store.send(path, a);
        } catch (exception& e) {
          warn << "Failed to send to store: " << e.what();
          continue;
        }
      }

      _index.save(a);
      changed = true;
    }
  }

  // check for deleted nodes
  for (auto& a : stored.list()) {
    const Node* found = nullptr;
    for (auto& n : nl.list()) {
      if (a.name() == n.name()) {
        found = &a;
      }
    }
    if (found == nullptr) {
      info << "\x1b[31m-\x1b[0m " << a.path() << "/" << a.name()
           << (a.is_dir() ? "/" : "");
      _index.deleted(a, nl.mtime());
      changed = true;
    }
  }

  if (changed) {
    _index.flush();
  }

  return changed;
}

vector<string> Backitup::list_path(string path) {
  auto nl = _index.latest(path);

  vector<string> out;
  unsigned long total_bytes = 0;

  vector<Node> vec = nl.list();

  sort(vec.begin(), vec.end(), less<Node>());

  for (auto& n : vec) {
    stringstream ss;

    time_t mtime_time_t = n.mtime();

    char mbstr[100];
    if (std::strftime(mbstr, sizeof(mbstr), "%b %e %R %Y",
                      std::localtime(&mtime_time_t))) {
    }

    if (n.is_dir()) {
      ss << setfill(' ') << setw(10) << "";
    } else {
      ss << setfill(' ') << setw(10) << n.size();
    }

    ss << " " << mbstr << " ";
    if (n.is_dir()) {
      ss << "\x1b[34m" << n.name() << "\x1b[0m";
    } else {
      ss << n.name();
    }
    out.push_back(ss.str());

    total_bytes += n.size();
  }

  {
    stringstream header;
    header << "total " << (total_bytes / 1024 / 1024) << " MB";
    out.insert(out.begin(), header.str());
  }

  return out;
}

void Backitup::restore(string path, string dest) {
  auto nl = _index.latest(path);

  for (auto& n : nl.list()) {
    if (n.is_dir()) {
      string new_path = trim_slashes(path + "/" + n.name());
      string new_dest = trim_slashes(dest + "/" + n.name());
      fs::create_directories(new_dest);
      restore(new_path, new_dest);
    } else {
      info << "Restoring " << n.full_path();
      _store.retrieve(n, dest);
    }
  }
}
}

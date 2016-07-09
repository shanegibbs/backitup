/*
 * BackupPath.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <chrono>
#include <condition_variable>
#include <ctime>
#include <dirent.h>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <thread>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

#include "BackupPath.h"
#include "Log.h"
#include "Node.h"
#include "Record.h"

namespace fs = boost::filesystem;

namespace backitup {

static Log LOG = Log("BackupPath");

BackupPath::BackupPath(const std::string p, vector<string> e)
    : path(boost::filesystem::canonical(p).native()),
      excludes(e),
      _watch_started(false) {}

BackupPath::~BackupPath() { stop(); }

void BackupPath::stop() {
  if (_watch_started) {
    FSEventStreamFlushSync(_stream);
    FSEventStreamStop(_stream);
    FSEventStreamInvalidate(_stream);
    FSEventStreamRelease(_stream);
    CFRunLoopStop(_run_loop_ptr);
    _watch_thread.join();
    _watch_started = false;
  }
}

// TODO convert to bredth first

void BackupPath::visit(
    function<void(const string &, const NodeList &)> fn) const {
  // visit(string(""), fn);

  // we do bredth first search so that we do not pass a file before
  // it's directory

  queue<string> todo;
  todo.push("");

  while (!todo.empty()) {
    string p = todo.front();
    todo.pop();

    auto nl = list(p);
    fn(path, nl);

    for (auto &n : nl.list()) {
      if (n.is_dir()) {
        todo.push(n.full_path());
      }
    }
  }
}

NodeList BackupPath::list(const string &p) const {
  NodeList nl(p);
  nl.mtime(time(nullptr));

  struct stat s;
  DIR *d;
  struct dirent *dir;
  string full_path = path + "/" + p;

  d = opendir(full_path.c_str());
  if (!d) {
    if (fs::exists(full_path)) {
      warn << "Unable to read directory " << full_path;
    }
    return nl;
  }

  while ((dir = readdir(d)) != NULL) {
    if (strcmp(dir->d_name, ".") == 0) continue;
    if (strcmp(dir->d_name, "..") == 0) continue;

    string filename = full_path + "/" + dir->d_name;

    if (dir->d_type == DT_DIR) {
      string next_p = (!p.empty() ? p + "/" : "") + dir->d_name;
      Node d_node(p, dir->d_name, true);

      if (stat(filename.c_str(), &s) == -1) {
        warn << "Unable to stat directory: " << filename;
      } else {
        long mtime = s.st_mtime;
        long ctime = s.st_ctime;
        if (ctime > mtime) mtime = ctime;
        // Node node(p, string(dir->d_name), mtime, s.st_size, string(""));
        // nl.add(node);
        d_node.mtime(mtime);
      }

      nl.add(d_node);

    } else if (dir->d_type == DT_REG) {
      if (stat(filename.c_str(), &s) == -1) {
        warn << "Unable to stat file: " << filename;
      } else {
        long mtime = s.st_mtime;
        long ctime = s.st_ctime;
        if (ctime > mtime) mtime = ctime;
        Node node(p, string(dir->d_name), mtime, s.st_size, string(""));
        nl.add(node);
      }
    }
  }

  closedir(d);
  return nl;
}

void BackupPath::_watch_callback(const string p) {
  string updated_path = p;
  if (updated_path.back() == '/') {
    updated_path = updated_path.substr(0, updated_path.size() - 1);
  }

  // stop if we hit an excludes
  for (auto &e : excludes) {
    if (updated_path == e) {
      return;
    }
  }

  // remove base path prefix
  updated_path = updated_path.substr(path.length());

  if (updated_path.front() == '/') {
    updated_path = updated_path.substr(1);
  }
  if (updated_path.back() == '/') {
    updated_path = updated_path.substr(0, updated_path.size() - 1);
  }

  _on_watch_event(updated_path);
}

static void watch_callback(ConstFSEventStreamRef streamRef,
                           void *clientCallBackInfo, size_t numEvents,
                           void *eventPaths,
                           const FSEventStreamEventFlags eventFlags[],
                           const FSEventStreamEventId eventIds[]) {
  char **paths = (char **)eventPaths;

  BackupPath *p = (BackupPath *)clientCallBackInfo;

  for (int i = 0; i < numEvents; i++) {
    p->_watch_callback(string(paths[i]));
  }
}

void BackupPath::watch(function<void(const string &changed)> fn) {
  if (_watch_started) {
    throw "Watch alraedy running";
  }
  _watch_started = true;
  _on_watch_event = fn;

  mutex m;
  condition_variable cv;
  bool init_complete = false;

  _watch_thread = thread([&]() {
    CFStringRef mypath =
        CFStringCreateWithCString(NULL, path.c_str(), kCFStringEncodingUTF8);
    CFArrayRef pathsToWatch =
        CFArrayCreate(NULL, (const void **)&mypath, 1, NULL);

    struct FSEventStreamContext *context =
        (struct FSEventStreamContext *)calloc(
            sizeof(struct FSEventStreamContext), 1);

#pragma push_macro("info")
#undef info
    context->info = this;
#pragma pop_macro("info")

    CFAbsoluteTime latency = 0.0; /* Latency in seconds */

    _run_loop_ptr = CFRunLoopGetCurrent();

    /* Create the stream, passing in a callback */
    _stream = FSEventStreamCreate(
        NULL, &watch_callback, context, pathsToWatch,
        kFSEventStreamEventIdSinceNow,        /* Or a previous event ID */
        latency, kFSEventStreamCreateFlagNone /* Flags explained in reference */
        );
    FSEventStreamScheduleWithRunLoop(_stream, _run_loop_ptr,
                                     kCFRunLoopDefaultMode);
    FSEventStreamStart(_stream);

    {
      unique_lock<mutex> lk(m);
      init_complete = true;
      cv.notify_one();
    }

    CFRunLoopRun();
  });

  unique_lock<mutex> lk(m);
  cv.wait_for(lk, 5s, [&] { return init_complete; });
  if (!init_complete) {
    throw "Timeout waiting for FSEvent stream to start";
  }
}
}

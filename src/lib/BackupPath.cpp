/*
 * BackupPath.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <ctime>
#include <dirent.h>
#include <functional>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <thread>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

#include "BackupPath.h"
#include "Node.h"
#include "Record.h"

namespace backitup {

BackupPath::BackupPath(const std::string p, vector<string> e)
    : path(boost::filesystem::canonical(p).native()),
      excludes(e),
      _watch_started(false) {}

BackupPath::~BackupPath() {
  if (_watch_started) {
    FSEventStreamFlushSync(_stream);
    FSEventStreamStop(_stream);
    FSEventStreamInvalidate(_stream);
    FSEventStreamRelease(_stream);
  }
}

void BackupPath::visit(
    function<void(const string &, const NodeList &)> fn) const {
  visit(string(""), fn);
}

void BackupPath::visit(
    const string &p,
    function<void(const string &path, const NodeList &)> fn) const {
  DIR *d;
  struct dirent *dir;
  string full_path = path + "/" + p;

  NodeList nl(p);

  // stop if we hit an excludes
  for (auto &e : excludes) {
    if (full_path == e) {
      cout << "Skipping " << full_path << endl;
      return;
    }
  }

  d = opendir(full_path.c_str());
  if (!d) {
    cerr << "WARN: Unable to read directory " << full_path << endl;
    return;
  }

  while ((dir = readdir(d)) != NULL) {
    if (strcmp(dir->d_name, ".") == 0) continue;
    if (strcmp(dir->d_name, "..") == 0) continue;

    if (dir->d_type == DT_DIR) {
      string next_p = (!p.empty() ? p + "/" : "") + dir->d_name;
      visit(next_p, fn);
    } else if (dir->d_type == DT_REG) {
      string filename = full_path + "/" + dir->d_name;
      struct stat s;
      if (stat(filename.c_str(), &s) == -1) {
        cerr << "WARN: Unable to stat file: " << filename << endl;
      } else {
        long mtime = s.st_mtime;
        long ctime = s.st_ctime;
        if (ctime > mtime) mtime = ctime;
        Node node(p, string(dir->d_name), mtime, s.st_size, string(""));
        nl.add(node);
      }
    }
  }
  fn(path, nl);
}

struct WatchContext {
  function<void(const string &changed)> fn;
  string path;
  vector<string> excludes;
};

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

  function<void(const string &changed)> fnn = fn;

  std::thread t([&]() {
    CFStringRef mypath =
        CFStringCreateWithCString(NULL, path.c_str(), kCFStringEncodingUTF8);
    CFArrayRef pathsToWatch =
        CFArrayCreate(NULL, (const void **)&mypath, 1, NULL);

    struct FSEventStreamContext *context =
        (struct FSEventStreamContext *)calloc(
            sizeof(struct FSEventStreamContext), 1);

    /*
    auto info = (struct WatchContext *)calloc(sizeof(struct WatchContext), 1);
    info->fn = fn;
    info->excludes = excludes;
    info->path = boost::filesystem::canonical(path).native();
    */

    context->info = this;

    CFAbsoluteTime latency = 0.0; /* Latency in seconds */

    /* Create the stream, passing in a callback */
    _stream = FSEventStreamCreate(
        NULL, &watch_callback, context, pathsToWatch,
        kFSEventStreamEventIdSinceNow,        /* Or a previous event ID */
        latency, kFSEventStreamCreateFlagNone /* Flags explained in reference */
        );
    FSEventStreamScheduleWithRunLoop(_stream, CFRunLoopGetCurrent(),
                                     kCFRunLoopDefaultMode);
    FSEventStreamStart(_stream);
    CFRunLoopRun();
  });
  t.detach();
}

void BackupPath::visitFilesRecursive(
    const string &base, shared_ptr<Node> node,
    function<void(const string &, shared_ptr<Node>)> fn) const {
  DIR *d;
  struct dirent *dir;
  string fullPath = base + node->getFullPath();

  // stop if we hit an excludes
  for (auto &e : excludes) {
    if (fullPath == e) {
      cout << "Skipping " << fullPath << endl;
      return;
    }
  }

  const string child_path = node->getFullPath();

  d = opendir(fullPath.c_str());
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (strcmp(dir->d_name, ".") == 0) continue;
      if (strcmp(dir->d_name, "..") == 0) continue;
      shared_ptr<Node> child(new Node(0, dir->d_name, node));
      child->path(child_path);

      node->addChild(child);

      if (dir->d_type == DT_DIR) {
        child->mtime(0);
        child->size(0);
        // fn(path, child); Skip DIRs for now
        visitFilesRecursive(base, child, fn);

      } else if (dir->d_type == DT_REG) {
        string filename = base + child->getFullPath();
        struct stat s;
        if (stat(filename.c_str(), &s) == -1) {
          cout << "Failed to read " << filename << endl;
        } else {
#if __APPLE__ && __MACH__
          long mtime = s.st_mtime;
          long ctime = s.st_ctime;

          if (ctime > mtime) mtime = ctime;

          child->mtime(mtime);
          child->size(s.st_size);
#else
#error TODO Linux support for mtime
#endif
        }

        fn(path, child);
      }
    }
    closedir(d);
  }
}

struct WatcherContext {
  long lastUpdate;
  function<void(const string &, NodeListRef)> fn;
  string base_path;
  string path;
  vector<string> excludes;
};

shared_ptr<Node> BackupPath::visitFiles(
    function<void(const string &, shared_ptr<Node>)> fn) const {
  shared_ptr<Node> root = Node::createRoot();
  visitFilesRecursive(path, Node::createRoot(), fn);
  return root;
}

static void mycallback(ConstFSEventStreamRef streamRef,
                       void *clientCallBackInfo, size_t numEvents,
                       void *eventPaths,
                       const FSEventStreamEventFlags eventFlags[],
                       const FSEventStreamEventId eventIds[]) {
  int i;
  char **paths = (char **)eventPaths;

  auto info = (struct WatcherContext *)clientCallBackInfo;

  // printf("Callback called\n");
  for (i = 0; i < numEvents; i++) {
    /* flags are unsigned long, IDs are uint64_t */
    // printf("Change %llu in %s, flags %lu\n", eventIds[i], paths[i],
    //        eventFlags[i]);

    // cout << "PING " << paths[i] << endl;

    // cout << hex << eventFlags[i] << dec << endl;

    /*
    if (eventFlags[i] == kFSEventStreamEventFlagNone) {
      cout << "None" << endl;
    }
    */
    if ((eventFlags[i] & kFSEventStreamEventFlagItemCreated) ==
        kFSEventStreamEventFlagItemCreated) {
      cout << "Created" << endl;
    }
    if ((eventFlags[i] & kFSEventStreamEventFlagItemRemoved) ==
        kFSEventStreamEventFlagItemRemoved) {
      cout << "Removed" << endl;
    }
    if ((eventFlags[i] & kFSEventStreamEventFlagItemModified) ==
        kFSEventStreamEventFlagItemModified) {
      cout << "Modified" << endl;
    }
    if ((eventFlags[i] & kFSEventStreamEventFlagItemRenamed) ==
        kFSEventStreamEventFlagItemRenamed) {
      cout << "Renamed" << endl;
    }
    if ((eventFlags[i] & kFSEventStreamEventFlagItemIsFile) ==
        kFSEventStreamEventFlagItemIsFile) {
      cout << "IsFile" << endl;
    }
    if ((eventFlags[i] & kFSEventStreamEventFlagItemIsDir) ==
        kFSEventStreamEventFlagItemIsDir) {
      cout << "IsDir" << endl;
    }
    if ((eventFlags[i] & kFSEventStreamEventFlagItemIsSymlink) ==
        kFSEventStreamEventFlagItemIsSymlink) {
      cout << "IsSym" << endl;
    }

    string updated_path(paths[i]);
    if (updated_path.back() == '/') {
      updated_path = updated_path.substr(0, updated_path.size() - 1);
    }

    // stop if we hit an excludes
    for (auto &e : info->excludes) {
      if (updated_path == e) {
        return;
      }
    }

    // remove base path prefix
    updated_path = string(paths[i]).substr(info->path.length());

    auto nl = NodeList::New(updated_path);
    {
      struct stat s;
      if (stat(paths[i], &s) == -1) {
        cout << "Failed to read " << paths[i] << endl;
      } else {
        nl->mtime(s.st_mtime);
        // cout << "mtime of " << paths[i] << ": " << s.st_mtime << endl;
      }
    }

    DIR *d;
    struct dirent *dir;
    d = opendir(paths[i]);
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, ".") == 0) continue;
        if (strcmp(dir->d_name, "..") == 0) continue;

        if (dir->d_type == DT_DIR) {
          // rs.addDir(string(dir->d_name));
        } else if (dir->d_type == DT_REG) {
          string filename = string(paths[i]) + string(dir->d_name);
          Node n(0, string(dir->d_name), nullptr);
          n.path(updated_path);
          struct stat s;
          if (stat(filename.c_str(), &s) == -1) {
            cout << "Failed to read " << filename << endl;
          } else {
#if __APPLE__ && __MACH__
            long mtime = s.st_mtime;
            long ctime = s.st_ctime;

            if (ctime > mtime) mtime = ctime;

            n.size(s.st_size);
            n.mtime(mtime);

#else
#error TODO Linux support for mtime
#endif
            nl->add(n);
          }
        }
      }
      closedir(d);

      info->fn(info->base_path, nl);
    }
  }
}

void BackupPath::watchFiles(
    function<void(const string &, NodeListRef)> fn) const {
  std::thread t([&]() {
    long int t = static_cast<long int>(time(NULL));

    CFStringRef mypath =
        CFStringCreateWithCString(NULL, path.c_str(), kCFStringEncodingUTF8);
    CFArrayRef pathsToWatch =
        CFArrayCreate(NULL, (const void **)&mypath, 1, NULL);

    // struct WatcherContext *context = NULL;
    // (struct WatcherContext *)malloc(sizeof(struct WatcherContext));
    // context->lastUpdate = 0;

    // void *callbackInfo = 0;  // could put stream-specific data here.

    struct FSEventStreamContext *context =
        (struct FSEventStreamContext *)calloc(
            sizeof(struct FSEventStreamContext), 1);

    auto info =
        (struct WatcherContext *)calloc(sizeof(struct WatcherContext), 1);
    info->lastUpdate = t;
    info->fn = fn;
    info->base_path = path;
    info->excludes = excludes;

    info->path = boost::filesystem::canonical(path).native();

    context->info = info;

    FSEventStreamRef stream;
    CFAbsoluteTime latency = 0.0; /* Latency in seconds */

    /* Create the stream, passing in a callback */
    stream = FSEventStreamCreate(
        NULL, &mycallback, context, pathsToWatch,
        kFSEventStreamEventIdSinceNow,        /* Or a previous event ID */
        latency, kFSEventStreamCreateFlagNone /* Flags explained in reference */
        );
    FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(),
                                     kCFRunLoopDefaultMode);
    FSEventStreamStart(stream);
    CFRunLoopRun();
  });
  t.detach();
}
}

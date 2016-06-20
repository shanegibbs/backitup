/*
 * BackupPath.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

/* MAC */
#include <CoreServices/CoreServices.h>

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

BackupPath::BackupPath(const std::string p) : path(p) {}

/*
static string ts_to_string(long ts) {
  std::time_t now = (const time_t)ts;
  std::tm *ptm = std::localtime(&now);
  char buffer[32];
  std::strftime(buffer, 32, "%a, %Y-%m-%d %H:%M:%S", ptm);
  return string(buffer);
}
*/

void BackupPath::visitFilesRecursive(
    const string &base, shared_ptr<Node> node,
    function<void(const string &, shared_ptr<Node>)> fn) const {
  DIR *d;
  struct dirent *dir;
  string fullPath = base + node->getFullPath();
  d = opendir(fullPath.c_str());
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (strcmp(dir->d_name, ".") == 0) continue;
      if (strcmp(dir->d_name, "..") == 0) continue;
      shared_ptr<Node> child(new Node(0, dir->d_name, node));
      node->addChild(child);

      if (dir->d_type == DT_DIR) {
        child->mtime(0);
        child->size(0);
        fn(path, child);
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
  function<void(NodeListRef)> fn;
  string path;
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

    // cout << "info->path.length() = " << info->path.length() << endl;
    auto updated_path = string(paths[i]).substr(info->path.length());

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

      info->fn(nl);
    }
  }
}

void BackupPath::watchFiles(function<void(NodeListRef)> fn) const {
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

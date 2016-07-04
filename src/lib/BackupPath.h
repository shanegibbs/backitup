/*
 * BackupPath.h
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#ifndef BACKUPPATH_H_
#define BACKUPPATH_H_

#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>

/* MAC */
#include <CoreServices/CoreServices.h>

#include "Node.h"

using namespace std;

namespace backitup {

class Node;
class FileIndex;

class BackupPath {
 public:
  BackupPath(const string path, vector<string> e);
  ~BackupPath();

  std::thread watch(function<void(const string &changed)> fn);

  void visit(function<void(const string &path, const NodeList &)> fn) const;
  void visit(const string &p,
             function<void(const string &path, const NodeList &)> fn) const;

  void _watch_callback(const string updated_path);

  NodeList list(const string &path) const;

  const string get_path() const { return path; }

 private:
  const string path;
  vector<string> excludes;

  bool _watch_started;
  function<void(const string &changed)> _on_watch_event;

  // MAC
  FSEventStreamRef _stream;
};
}

#endif /* BACKUPPATH_H_ */

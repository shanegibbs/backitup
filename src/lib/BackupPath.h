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

  shared_ptr<Node> visitFiles(
      function<void(const string &path, shared_ptr<Node>)> fn) const;

  void watchFiles(function<void(const string &path, NodeListRef)> fn) const;

  void watch(function<void(const string &changed)> fn);

  void visit(function<void(const string &path, const NodeList &)> fn) const;
  void visit(const string &p,
             function<void(const string &path, const NodeList &)> fn) const;

  void _watch_callback(const string updated_path);

 private:
  void visitFilesRecursive(
      const string &base, shared_ptr<Node> node,
      function<void(const string &, shared_ptr<Node>)> fn) const;

  const string path;
  vector<string> excludes;

  bool _watch_started;
  function<void(const string &changed)> _on_watch_event;

  // MAC
  FSEventStreamRef _stream;
};
}

#endif /* BACKUPPATH_H_ */

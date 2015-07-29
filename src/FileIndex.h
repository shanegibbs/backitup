/*
 * FileIndex.h
 *
 *  Created on: Jul 20, 2015
 *      Author: sgibbs
 */

#ifndef FILEINDEX_H_
#define FILEINDEX_H_

#include <memory>
#include <string>

using namespace std;

class Db;

namespace google {
namespace protobuf {
class Message;
}
}

namespace backitup {

class Node;
class NodeRecord;

class FileIndex {
 public:
  FileIndex(const string &name);
  ~FileIndex();

  static shared_ptr<FileIndex> create(const string &name) {
    return shared_ptr<FileIndex>(new FileIndex(name));
  }

  const string &getName() { return name; }

  shared_ptr<Db> getDb() { return db; }

  void saveNodeRecord(const NodeRecord &r);
  void getNodeRecord(const ::google::protobuf::Message &k);

  shared_ptr<Node> getNode(unsigned int parentId, const string &name);

 private:
  const string &name;
  shared_ptr<Db> db;
};
}

#endif /* FILEINDEX_H_ */

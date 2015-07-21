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

namespace backitup {

class FileIndex {

public:
  FileIndex(const string &name);
  ~FileIndex();

  static shared_ptr<FileIndex> create(const string &name) {
    return shared_ptr<FileIndex>(new FileIndex(name));
  }

  const string &getName() {
    return name;
  }

  void getOrCreate();

private:
  const string &name;
  unique_ptr<Db> db;

};
}

#endif /* FILEINDEX_H_ */

/*
 * RepositoryIndex.h
 *
 *  Created on: Jul 28, 2015
 *      Author: sgibbs
 */

#ifndef REPOSITORY_INDEX_H_
#define REPOSITORY_INDEX_H_

#include <memory>
#include <string>

#include "Repository.h"

using namespace std;

class Db;

namespace backitup {

template <class K, class V, class I>
class RepositoryIndex {
 public:
  static shared_ptr<RepositoryIndex> create(shared_ptr<Repository<K, V>> repo);

  const string getRecord(const string &key);

  void close();

 private:
  RepositoryIndex(shared_ptr<Repository<K, V>> repo);

  void openDb();

  const string filename;
  shared_ptr<Db> idx;
  shared_ptr<Repository<K, V>> repo;
};

}
#endif /* INDEX_H_ */

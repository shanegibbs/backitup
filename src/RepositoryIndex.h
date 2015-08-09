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
class Dbt;

namespace backitup {

template <class K, class V, class I>
class RepositoryIndex {
 public:
  static shared_ptr<RepositoryIndex> create(const string &filename, shared_ptr<Repository<K, V>> repo);

  shared_ptr<V> get(const I &i);

  void close();

 private:
  RepositoryIndex(const string &filename, shared_ptr<Repository<K, V>> repo);

  int keyCreationCallback(Db *sdbp, const Dbt *pkey, const Dbt *pdata, Dbt *skey);

  const string filename;
  shared_ptr<Database> db;
};

}
#endif /* INDEX_H_ */

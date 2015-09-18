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
#include "DataHelperFn.h"

using namespace std;

class Db;
class Dbt;

namespace backitup {

typedef int(KeyExtractorFunc)(Db *, const Dbt *, const Dbt *, Dbt *);

template <class K, class V, class I>
class RepositoryIndex {
 public:

  static shared_ptr<RepositoryIndex> create(shared_ptr<Database> db,
                                            shared_ptr<Repository<K, V>> repo,
                                            KeyExtractorFunc extrator);

  static shared_ptr<RepositoryIndex> create(const string &filename,
                                            shared_ptr<Repository<K, V>> repo,
                                            KeyExtractorFunc extrator);

  shared_ptr<V> get(const I &i);

  void close();

 private:
  RepositoryIndex(shared_ptr<Database> db, shared_ptr<Repository<K, V>> repo,
                  KeyExtractorFunc extrator);
  RepositoryIndex(const string &filename, shared_ptr<Repository<K, V>> repo,
                  KeyExtractorFunc extrator);

  int keyCreationCallback(Db *sdbp, const Dbt *pkey, const Dbt *pdata,
                          Dbt *skey);

  const string filename;
  shared_ptr<Database> db;
};

int myindex(Db *sdbp, const Dbt *pkey, const Dbt *pdata, Dbt *skey);
}
#endif /* REPOSITORY_INDEX_H_ */

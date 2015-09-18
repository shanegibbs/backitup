/*
 * RepositoryIndex.cpp
 *
 *  Created on: Jul 28, 2015
 *      Author: sgibbs
 */

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <string>
#include <iostream>
#include DB_CXX_HEADER

#include "Log.h"
#include "Database.h"
#include "Database.pb.h"
#include "Repository.h"
#include "RepositoryIndex.h"
#include "DataHelperFn.h"

namespace backitup {


template <class K, class V, class I>
shared_ptr<RepositoryIndex<K, V, I>> RepositoryIndex<K, V, I>::create(
    shared_ptr<Database> db, shared_ptr<Repository<K, V>> repo,
    KeyExtractorFunc extrator) {
  assert(db);
  auto index = shared_ptr<RepositoryIndex<K, V, I>>(
      new RepositoryIndex<K, V, I>(db, repo, extrator));
  return index;
}

template <class K, class V, class I>
shared_ptr<RepositoryIndex<K, V, I>> RepositoryIndex<K, V, I>::create(
    const string &filename, shared_ptr<Repository<K, V>> repo,
    KeyExtractorFunc extrator) {
  assert(repo);
  auto index = shared_ptr<RepositoryIndex<K, V, I>>(
      new RepositoryIndex<K, V, I>(filename, repo, extrator));
  return index;
}

template <class K, class V, class I>
RepositoryIndex<K, V, I>::RepositoryIndex(shared_ptr<Database> db,
                                          shared_ptr<Repository<K, V>> repo,
                                          KeyExtractorFunc extrator) : db(db) {
  assert(db);
  assert(repo);
  repo->getDb()->getDb()->associate(NULL, db->getDb().get(), extrator, 0);
}

template <class K, class V, class I>
RepositoryIndex<K, V, I>::RepositoryIndex(const string &filename,
                                          shared_ptr<Repository<K, V>> repo,
                                          KeyExtractorFunc extrator)
    : filename(filename) {

  assert(repo);
  db = Database::open(filename);

  repo->getDb()->getDb()->associate(NULL, db->getDb().get(), extrator, 0);
}

template <class K, class V, class I>
void RepositoryIndex<K, V, I>::close() {
  assert(db);
  db->close();
}

template <class K, class V, class I>
shared_ptr<V> RepositoryIndex<K, V, I>::get(const I &i) {
  string input;
  i.SerializeToString(&input);

  // assert(db);
  auto output = db->getRecord(input);

  shared_ptr<V> v(new V());
  v->ParseFromString(output);

  return v;
}

template class backitup::RepositoryIndex<DatabaseSimpleKey, NodeRecord,
                                         ParentNameIndex>;
}

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

#include "Database.h"
#include "Database.pb.h"
#include "Repository.h"
#include "RepositoryIndex.h"

namespace backitup {

template <class K, class V, class I>
shared_ptr<RepositoryIndex<K, V, I>> RepositoryIndex<K, V, I>::create(
    const string &filename, shared_ptr<Repository<K, V>> repo) {
  auto index = shared_ptr<RepositoryIndex<K, V, I>>(
      new RepositoryIndex<K, V, I>(filename, repo));
  return index;
}

class Something {
 public:
  static int myindex(Db *sdbp, const Dbt *pkey, const Dbt *pdata, Dbt *skey) {

  // parse the primary key
  string pkeyString((char *)pkey->get_data(), pkey->get_size());
  DatabaseSimpleKey dsk;
  dsk.ParseFromString(pkeyString);

  cout << endl;
  cout << "* pkey.id=" << dsk.id() << endl;

  // parse data value
  string pdataString((char *)pdata->get_data(), pdata->get_size());
  NodeRecord nr;
  nr.ParseFromString(pdataString);

  cout << "* nr.name=" << nr.name() << ",nr.parentid=" << nr.parentid() << endl;

  // new secondary index
  ParentNameIndex idx;
  idx.set_parentid(nr.parentid());
  idx.set_name(nr.name());

  string idxString;
  idx.SerializeToString(&idxString);

  void *buffer = malloc(sizeof(char) * idxString.size());
  copy(idxString.begin(), idxString.end(), (char*)buffer);

  skey->set_data(buffer);
  skey->set_size(idxString.length());
  skey->set_flags(DB_DBT_APPMALLOC);

  return 0;
}
};

template <class K, class V, class I>
RepositoryIndex<K, V, I>::RepositoryIndex(const string &filename, shared_ptr<Repository<K, V>> repo)
    : filename(filename) {
  db = Database::open(filename);

  // TODO need to pass function here
  repo->getDb()->getDb()->associate(NULL, db->getDb().get(),
      Something::myindex, 0);
}

template <class K, class V, class I>
void RepositoryIndex<K, V, I>::close() {
  db->close();
}

template <class K, class V, class I>
shared_ptr<V> RepositoryIndex<K, V, I>::get(const I &i) {
  string input;
  i.SerializeToString(&input);

  auto output = db->getRecord(input);

  shared_ptr<V> v(new V());
  v->ParseFromString(output);

  return v;
}

template class backitup::RepositoryIndex<DatabaseSimpleKey, NodeRecord, ParentNameIndex>;
}

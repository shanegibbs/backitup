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
    shared_ptr<Repository<K, V>> repo) {

  auto index = shared_ptr<RepositoryIndex<K, V, I>>(
      new RepositoryIndex<K, V, I>(repo));

  return index;
}

template <class K, class V, class I>
RepositoryIndex<K, V, I>::RepositoryIndex(shared_ptr<Repository<K, V>> repo)
    : repo(repo) {}

template <class K, class V, class I>
void RepositoryIndex<K, V, I>::openDb() {
  u_int32_t oFlags = DB_CREATE;

  idx = shared_ptr<Db>(new Db(NULL, 0));
  idx->set_flags(DB_DUPSORT);

  try {
    // Open the database
    idx->open(NULL,              // Transaction pointer
              filename.c_str(),  // Database file name
              NULL,              // Optional logical database name
              DB_BTREE,          // Database access method
              oFlags,            // Open flags
              0);                // File mode (using defaults)
    // DbException is not subclassed from std::exception, so
    // need to catch both of these.
  } catch (DbException &e) {
    throw e;  // Error handling code goes here
  } catch (std::exception &e) {
    throw e;  // Error handling code goes here
  }
}

template <class K, class V, class I>
void RepositoryIndex<K, V, I>::close() {
  idx->close(0);
}

template <class K, class V, class I>
const string RepositoryIndex<K, V, I>::getRecord(const string &keyBuffer) {
  Dbt key((void *)keyBuffer.data(), keyBuffer.length());

  int size = 1024;
  shared_ptr<void> buffer(malloc(size), free);

  Dbt data;
  data.set_data(buffer.get());
  data.set_ulen(size);
  data.set_flags(DB_DBT_USERMEM);

  int ret = idx->get(NULL, &key, &data, 0);
  if (ret == DB_NOTFOUND) {
    throw NotFoundDatabaseException();
  } else if (ret != 0) {
    throw GeneralDatabaseException(ret);
  }

  string result((char *)data.get_data(), data.get_size());
  return result;
}

template class backitup::RepositoryIndex<DatabaseSimpleKey, NodeRecord, ParentNameIndex>;
}

/*
 * Repository.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: sgibbs
 */

#include <stdio.h>
#include <string.h>
#include <functional>
#include DB_CXX_HEADER

#include "Database.pb.h"
#include "Database.h"

#include "Repository.h"

namespace backitup {

template <class K, class V>
shared_ptr<Repository<K, V>> Repository<K, V>::create(const string &filename) {
  auto db = Database::open("index.db");
  return shared_ptr<Repository<K, V>>(new Repository<K, V>(db));
}

template <class K, class V>
shared_ptr<V> Repository<K, V>::get(const K &k) {
  auto keyBuf = keyToString(k);
  auto valBuf = db->getRecord(keyBuf);
  return stringToValue(valBuf);
}

template <class K, class V>
void Repository<K, V>::put(const K &k, const V &v) {
  auto keyBuf = keyToString(k);
  auto valueBuf = valueToString(v);
  cout << "keysize=" << keyBuf.length() << " valuesze=" << valueBuf.length() << endl;
  db->putRecord(keyBuf, valueBuf);
}

template <class K, class V>
const string Repository<K, V>::keyToString(const K &k) {
  string str;
  k.SerializeToString(&str);
  return str;
}

template <class K, class V>
const string Repository<K, V>::valueToString(const V &v) {
  string str;
  v.SerializeToString(&str);
  return str;
}

template <class K, class V>
shared_ptr<V> Repository<K, V>::stringToValue(const string &data) {
  shared_ptr<V> v(new V());
  v->ParseFromString(data);
  return v;
}

template <class K, class V>
void Repository<K, V>::compact() {
  db->compact();
}

/*
template <class K, class V>
void Repository<K, V>::associate(shared_ptr<Database> secondary) {

  // function<int (Db *, const Dbt *, const Dbt *, Dbt *)> func;
  // function<int, (Db *sdbp, const Dbt *pkey, const Dbt *pdata,Dbt *skey)>;

  db->getDb()->associate(NULL, secondary->getDb().get(), 0, 0);
}
*/

template class backitup::Repository<DatabaseSimpleKey, CounterRecord>;
template class backitup::Repository<DatabaseSimpleKey, NodeRecord>;
}

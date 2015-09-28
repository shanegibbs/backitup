/*
 * Repository.h
 *
 *  Created on: Jul 22, 2015
 *      Author: sgibbs
 */

#ifndef REPOSITORY_H_
#define REPOSITORY_H_

#include <memory>
#include <string>

using namespace std;

namespace backitup {

class Database;

// Wrapper class that allows to get and put
// protobuf objects
template <class K, class V>
class Repository {
 public:
  Repository(shared_ptr<Database> db) : db(db){};

  static shared_ptr<Repository<K, V>> create(const string &filename);

  static shared_ptr<Repository<K, V>> create(shared_ptr<Database> db) {
    return shared_ptr<Repository<K, V>>(new Repository<K, V>(db));
  }

  shared_ptr<V> get(const K &k);
  shared_ptr<V> getKey(const K &k);

  void put(const K &k, const V &v);

  void compact();

  shared_ptr<Database> getDb() {
    return db;
  }

 private:
  const string keyToString(const K &k);
  const string valueToString(const V &v);
  shared_ptr<V> stringToValue(const string &data);

  shared_ptr<Database> db;
};
}
#endif /* REPOSITORY_H_ */

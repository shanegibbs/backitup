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

template <class K, class V>
class Repository {
 public:
  Repository(shared_ptr<Database> db) : db(db){};

  static shared_ptr<Repository<K, V>> create(shared_ptr<Database> db) {
    return shared_ptr<Repository<K, V>>(new Repository<K, V>(db));
  }

  shared_ptr<V> get(const K &k);
  void put(const K &k, const V &v);

 private:
  const string keyToString(const K &k);
  const string valueToString(const V &v);
  shared_ptr<V> stringToValue(const string &data);

  shared_ptr<Database> db;
};

}
#endif /* REPOSITORY_H_ */

/*
 * NodeRepo.h
 *
 *  Created on: Jul 26, 2015
 *      Author: sgibbs
 */

#ifndef NODEREPO_H_
#define NODEREPO_H_

#include <memory>
#include <string>

#include "Database.pb.h"
#include "Repository.h"
#include "RepositoryIndex.h"

using namespace std;

class Db;
class Dbt;

namespace backitup {

class Database;
class Node;

class NodeRepo {
 public:
  NodeRepo(const string &name);

  // not const because we update the id field
  void save(Node &n);

  shared_ptr<Node> getById(unsigned int id);

  static int ParentNameIndexExtractor(Db *sdbp, const Dbt *pkey,
                                      const Dbt *pdata, Dbt *skey);

  void compact();

 private:
  const string name;
  shared_ptr<Database> db;
  shared_ptr<Repository<DatabaseSimpleKey, CounterRecord>> counter;
  shared_ptr<Repository<DatabaseSimpleKey, NodeRecord>> repo;
  shared_ptr<RepositoryIndex<DatabaseSimpleKey, NodeRecord, ParentNameIndex>>
      parentIdx;

  unsigned int nextId();
};
}

#endif /* NODEREPO_H_ */

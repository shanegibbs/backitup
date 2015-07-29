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

#include "Repository.h"
#include "Database.pb.h"

using namespace std;

namespace backitup {

class Database;
class Node;

class NodeRepo {
 public:
  NodeRepo(shared_ptr<Database> db)
      : counter(Repository<DatabaseSimpleKey, CounterRecord>(db)),
        repo(Repository<DatabaseSimpleKey, NodeRecord>(db)) {}

  // not const because we update the id field
  void save(Node &n);

 private:
  Repository<DatabaseSimpleKey, CounterRecord> counter;
  Repository<DatabaseSimpleKey, NodeRecord> repo;

  unsigned int nextId();
};
}

#endif /* NODEREPO_H_ */

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
  NodeRepo(const string &name);

  // not const because we update the id field
  void save(Node &n);

 private:
  const string name;
  shared_ptr<Database> db;
  shared_ptr<Repository<DatabaseSimpleKey, CounterRecord>> counter;
  shared_ptr<Repository<DatabaseSimpleKey, NodeRecord>> repo;

  unsigned int nextId();
};
}

#endif /* NODEREPO_H_ */

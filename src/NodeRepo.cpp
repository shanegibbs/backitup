/*
 * NodeRepo.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <sstream>
#include <iostream>
#include <stack>

#include "NodeRepo.h"
#include "Node.h"
#include "Database.h"
#include "Database.pb.h"
#include "RepositoryIndex.h"

namespace backitup {

NodeRepo::NodeRepo(const string &name) : name(name) {
  db = Database::open(name + ".db");
  counter = shared_ptr<Repository<DatabaseSimpleKey, CounterRecord>>(
      new Repository<DatabaseSimpleKey, CounterRecord>(db));
  repo = shared_ptr<Repository<DatabaseSimpleKey, NodeRecord>>(
      new Repository<DatabaseSimpleKey, NodeRecord>(db));

  // auto index = RepositoryIndex::open(name + "-index.db");
  // repo->addIndex(*index);
}

unsigned int NodeRepo::nextId() {
  // key of the counter
  DatabaseSimpleKey k;
  k.set_type(COUNTER);
  k.set_id(1);

  // counter record will go here
  shared_ptr<CounterRecord> r;

  try {
    // get existing counter and increment it
    r = counter->get(k);
    r->set_id(r->id() + 1);

  } catch (NotFoundDatabaseException e) {
    // create new counter and set to 1
    r = shared_ptr<CounterRecord>(new CounterRecord());
    r->set_id(1);
  }

  cout << "Next id is " << r->id() << endl;

  // save the new counter
  try {
    counter->put(k, *r);
  } catch (ExistsDatabaseException e) {
    cerr << "Unable to put key" << endl;
    abort();
  }

  // return the id
  return r->id();
}

void NodeRepo::save(Node &n) {
  // continue if node already saved
  if (n.getId() > 0) return;

  // first save parent node
  if (n.getParent()) save(*n.getParent());

  cout << "saving " << n.getName() << endl;

  // get existing record with parent and name

  // if not exist, insert

  unsigned int id = nextId();
  n.setId(id);

  DatabaseSimpleKey k;
  k.set_type(NODE);
  k.set_id(id);

  NodeRecord r;
  r.set_name(n.getName());
  r.set_leaf(true);

  repo->put(k, r);
}
}

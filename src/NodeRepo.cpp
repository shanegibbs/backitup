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

namespace backitup {

unsigned int NodeRepo::nextId() {
  DatabaseSimpleKey k;
  k.set_type(COUNTER);
  k.set_id(1);

  shared_ptr<CounterRecord> r;
  try {
    r = counter.get(k);
    r->set_id(r->id() + 1);
  } catch (NotFoundDatabaseException e) {
    r = shared_ptr<CounterRecord>(new CounterRecord());
    r->set_id(1);
  }

  cout << "Next id is " << r->id() << endl;

  try {
    counter.put(k, *r);
  } catch (ExistsDatabaseException e) {
    cerr << "Unable to put key" << endl;
    abort();
  }

  return r->id();
}

void NodeRepo::save(Node &n) {
  unsigned int id = nextId();
  n.setId(id);

  DatabaseSimpleKey k;
  k.set_type(NODE);
  k.set_id(id);

  NodeRecord r;
  r.set_name(n.getName());
  r.set_leaf(true);

  repo.put(k, r);
}
}

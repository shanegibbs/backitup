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
  auto counter_db = Database::open(name + "-counter.db");

  counter = shared_ptr<Repository<DatabaseSimpleKey, CounterRecord>>(
      new Repository<DatabaseSimpleKey, CounterRecord>(counter_db));

  repo = shared_ptr<Repository<DatabaseSimpleKey, NodeRecord>>(
      new Repository<DatabaseSimpleKey, NodeRecord>(db));

  parentIdx =
      RepositoryIndex<DatabaseSimpleKey, NodeRecord, ParentNameIndex>::create(
          name + "-index.db", repo, NodeRepo::ParentNameIndexExtractor);
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

  cout << "saving name='" << n.getName() << "'" << endl;

  bool found = false;

  // get existing record with parent id and our name if already exists
  ParentNameIndex p;
  p.set_name(n.getName());

  if (n.getParent()) {
    p.set_parentid(n.getParent()->getId());
  } else {
    p.set_parentid(0);
  }

  try {
    auto existingRecord = parentIdx->getValue(p);
    n.setId(existingRecord->first.id());

    cout << "Node " << n.getId() << " already exists" << endl;
    found = true;

    return;

  } catch (NotFoundDatabaseException e) {
    cout << "Node not in db yet" << endl;
  }

  // if not exist, insert

  unsigned int id = nextId();
  n.setId(id);

  DatabaseSimpleKey k;
  k.set_type(NODE);
  k.set_id(id);

  NodeRecord r;
  if (n.getParent()) {
    r.set_parentid(n.getParent()->getId());
  } else {
    r.set_parentid(0);
  }
  r.set_name(n.getName());
  r.set_leaf(true);

  cout << "Saving Node id:" << k.id() << " parentid:" << r.parentid()
       << " name:" << r.name() << endl;
  repo->put(k, r);
}

shared_ptr<Node> NodeRepo::getById(unsigned int id) {
  DatabaseSimpleKey k;
  k.set_type(NODE);
  k.set_id(id);

  auto nodeRecord = repo->get(k);

  if (nodeRecord->parentid() > 0) {
    auto parent = this->getById(nodeRecord->parentid());
    return Node::create(id, nodeRecord->name(), parent);
  } else {
    return Node::create(id, nodeRecord->name(), nullptr);
  }
}

void NodeRepo::compact() {
  counter->compact();
  repo->compact();
  parentIdx->compact();
}

int NodeRepo::ParentNameIndexExtractor(Db *sdbp, const Dbt *pkey,
                                       const Dbt *pdata, Dbt *skey) {
  // parse the primary key
  auto dsk = parse<DatabaseSimpleKey>(pkey);

  // cout << endl;
  // cout << "ParentNameIndexExtractor" << endl;
  // cout << "* pkey.id=" << dsk.id() << endl;

  // parse data value
  auto nr = parse<NodeRecord>(pdata);

  // cout << "* nr.name=" << nr.name() << ",nr.parentid=" << nr.parentid() <<
  // endl;

  // new secondary index
  ParentNameIndex idx;
  idx.set_parentid(nr.parentid());
  idx.set_name(nr.name());

  copyToDbt(idx, skey);

  return 0;
}
}

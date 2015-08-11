/*
 * FileIndexTest.cpp
 *
 *  Created on: Jul 20, 2015
 *      Author: sgibbs
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <cppunit/config/SourcePrefix.h>
#include DB_CXX_HEADER

#include "RepositoryTest.h"
#include <Repository.h>
#include <RepositoryIndex.h>
#include <Database.h>
#include <Database.pb.h>

using namespace backitup;

CPPUNIT_TEST_SUITE_REGISTRATION(RepositoryTest);

void RepositoryTest::setUp() {
  remove("index.db");

  db = Database::open("index.db");

}

void RepositoryTest::tearDown() {
  db->close();
  remove("index.db");
}

void RepositoryTest::testInsert() {
  auto r = Repository<DatabaseSimpleKey, NodeRecord>::create(db);

  DatabaseSimpleKey k;
  k.set_type(NODE);
  k.set_id(1);

  NodeRecord v;
  v.set_name("root");
  v.set_leaf(false);

  r->put(k, v);
}

void RepositoryTest::testGet() {
  auto r = Repository<DatabaseSimpleKey, NodeRecord>::create(db);

  DatabaseSimpleKey k;
  k.set_type(NODE);
  k.set_id(1);

  NodeRecord v;
  v.set_name("root");
  v.set_leaf(false);

  r->put(k, v);

  auto vv = r->get(k);
  CPPUNIT_ASSERT(vv);
  CPPUNIT_ASSERT_EQUAL(string("root"), vv->name());
}

void RepositoryTest::testGetNotFound() {
  auto r = Repository<DatabaseSimpleKey, NodeRecord>::create(db);

  DatabaseSimpleKey k;
  k.set_type(NODE);
  k.set_id(1);

  CPPUNIT_ASSERT_THROW(r->get(k), NotFoundDatabaseException);
}

void RepositoryTest::testIndex() {

  auto repo = Repository<DatabaseSimpleKey, NodeRecord>::create(db);
  auto idx = RepositoryIndex<DatabaseSimpleKey, NodeRecord, ParentNameIndex>::create("index", repo);

  DatabaseSimpleKey k;
  k.set_type(NODE);
  k.set_id(2);

  NodeRecord v;
  v.set_parentid(1);
  v.set_name("subdir");
  v.set_leaf(false);

  repo->put(k, v);

  ParentNameIndex i;
  i.set_parentid(1);
  i.set_name("subdir");

  idx->get(i);
}

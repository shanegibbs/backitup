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
#include <DataHelperFn.h>
#include <Repository.h>
#include <RepositoryIndex.h>
#include <Database.h>
#include <Database.pb.h>
#include <NodeRepo.h>

using namespace backitup;

CPPUNIT_TEST_SUITE_REGISTRATION(RepositoryTest);

void RepositoryTest::setUp() {
  remove("test.db");
  remove("test2.db");

  db = Database::open("test.db");
  db2 = Database::open("test2.db");
}

void RepositoryTest::tearDown() {
  db->close();
  db2->close();
  remove("test.db");
  remove("test2.db");
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
  auto idx =
      RepositoryIndex<DatabaseSimpleKey, NodeRecord, ParentNameIndex>::create(
          db2, repo, NodeRepo::ParentNameIndexExtractor);

  // first key/value
  DatabaseSimpleKey k1;
  k1.set_type(NODE);
  k1.set_id(2);
  NodeRecord v1;
  v1.set_parentid(1);
  v1.set_name("subdir");
  v1.set_leaf(false);
  repo->put(k1, v1);

  // dummy key/value
  DatabaseSimpleKey k2;
  k2.set_type(NODE);
  k2.set_id(3);
  NodeRecord v2;
  v2.set_parentid(1);
  v2.set_name("subdir2");
  v2.set_leaf(false);
  repo->put(k2, v2);

  // index that should return the first
  // key/value.
  ParentNameIndex i;
  i.set_parentid(1);
  i.set_name("subdir");

  // check we have the first key/value
  shared_ptr<NodeRecord> n = idx->get(i);
  CPPUNIT_ASSERT_EQUAL((unsigned int)1, n->parentid());
  CPPUNIT_ASSERT_EQUAL(string("subdir"), n->name());
  CPPUNIT_ASSERT_EQUAL(false, n->leaf());
}

void RepositoryTest::testParentNameIndexExtractor() {

  // key
  DatabaseSimpleKey pk;
  pk.set_type(NODE);
  pk.set_id(2);

  Dbt pkey;
  copyToDbt(pk, &pkey);

  // value
  NodeRecord data;
  data.set_parentid(1);
  data.set_name("subdir");
  data.set_leaf(false);

  Dbt pdata;
  copyToDbt(data, &pdata);

  // index
  Dbt skey;
  NodeRepo::ParentNameIndexExtractor(0, &pkey, &pdata, &skey);
  auto idx = parse<ParentNameIndex>(&skey);

  CPPUNIT_ASSERT_EQUAL((unsigned int)1, idx.parentid());
  CPPUNIT_ASSERT_EQUAL(string("subdir"), idx.name());
}

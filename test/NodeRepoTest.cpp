/*
 * NodeRepoTest.cpp
 *
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <cppunit/config/SourcePrefix.h>

#include "NodeRepoTest.h"
#include <NodeRepo.h>
#include <Node.h>

using namespace std;
using namespace backitup;

CPPUNIT_TEST_SUITE_REGISTRATION(NodeRepoTest);

void NodeRepoTest::setUp() {
  remove("test.db");
  remove("test-counter.db");
  remove("test-index.db");
}

void NodeRepoTest::tearDown() { remove("index.db"); }

void NodeRepoTest::testSaveRoot() {
  auto root = Node::createRoot();

  NodeRepo repo("test");
  repo.save(*root);
  repo.save(*root);

  CPPUNIT_ASSERT_EQUAL((unsigned int)1, root->getId());

  auto node = repo.getById(1);
  CPPUNIT_ASSERT_EQUAL((unsigned int)1, node->getId());
  CPPUNIT_ASSERT_EQUAL(string(""), node->getName());
}

void NodeRepoTest::testSave() {

  auto root = Node::createRoot();
  auto dir = Node::create(0, "dir", root);
  auto file = Node::create(0, "file", dir);

  NodeRepo repo("test");
  repo.save(*file);

  CPPUNIT_ASSERT_EQUAL((unsigned int)1, root->getId());
  CPPUNIT_ASSERT_EQUAL((unsigned int)2, dir->getId());
  CPPUNIT_ASSERT_EQUAL((unsigned int)3, file->getId());

  auto node = repo.getById(3);
  CPPUNIT_ASSERT_EQUAL((unsigned int)3, node->getId());
  CPPUNIT_ASSERT_EQUAL(string("file"), node->getName());

  node = repo.getById(2);
  CPPUNIT_ASSERT_EQUAL((unsigned int)2, node->getId());
  CPPUNIT_ASSERT_EQUAL(string("dir"), node->getName());

  node = repo.getById(1);
  CPPUNIT_ASSERT_EQUAL((unsigned int)1, node->getId());
  CPPUNIT_ASSERT_EQUAL(string(""), node->getName());

}


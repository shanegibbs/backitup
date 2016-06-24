#include "TextNodeRepoTest.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <cppunit/config/SourcePrefix.h>
#include DB_CXX_HEADER

using namespace backitup;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(TextNodeRepoTest);

void TextNodeRepoTest::setUp() { remove("scratch.txt.db"); }

void TextNodeRepoTest::tearDown() {}

void TextNodeRepoTest::testInsert() {
  TextNodeRepo repo;
  auto stored = repo.latest(string("/"));
  CPPUNIT_ASSERT_EQUAL(0UL, stored.list().size());

  repo.save(Node("/", "a", 0, 0, "abc"));
  stored = repo.latest(string("/"));
  CPPUNIT_ASSERT_EQUAL(1UL, stored.list().size());
  CPPUNIT_ASSERT(repo.contains(Node("/", "a", 0, 0, "abc")));
  CPPUNIT_ASSERT(repo.contains(Node("/", "a", 0, 0, "")));
  CPPUNIT_ASSERT(!repo.contains(Node("/", "a", 0, 0, "a")));
  CPPUNIT_ASSERT(!repo.contains(Node("/", "a", 1, 0, "abc")));
  CPPUNIT_ASSERT(!repo.contains(Node("/", "a", 1, 0, "")));
  CPPUNIT_ASSERT(!repo.contains(Node("/", "b", 0, 0, "abc")));

  repo.save(Node("/", "a", 1, 0, "abc"));
  stored = repo.latest(string("/"));
  CPPUNIT_ASSERT_EQUAL(1UL, stored.list().size());

  repo.save(Node("/", "b", 1, 0, "abc"));
  stored = repo.latest(string("/"));
  CPPUNIT_ASSERT_EQUAL(2UL, stored.list().size());

  repo.deleted(Node("/", "b", 1, 0, ""), 2);
  stored = repo.latest(string("/"));
  CPPUNIT_ASSERT_EQUAL(1UL, stored.list().size());

  repo.deleted(Node("/", "a", 1, 0, ""), 2);
  stored = repo.latest(string("/"));
  CPPUNIT_ASSERT_EQUAL(0UL, stored.list().size());

  repo.save(Node("/subdir", "a", 0, 0, "abc"));
  CPPUNIT_ASSERT_EQUAL(1UL, repo.latest(string("/subdir")).list().size());

  repo.deleted(Node("/subdir", "a", 0, 0, ""), 1);
  CPPUNIT_ASSERT_EQUAL(0UL, repo.latest(string("/subdir")).list().size());
}
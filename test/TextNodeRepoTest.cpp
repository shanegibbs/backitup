#include "TextNodeRepoTest.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <cppunit/config/SourcePrefix.h>
#include DB_CXX_HEADER

using namespace backitup;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(TextNodeRepoTest);

void TextNodeRepoTest::setUp() { remove("test.db"); }

void TextNodeRepoTest::tearDown() {}

void TextNodeRepoTest::testMain() {
  TextNodeRepo repo("test.db");
  auto stored = repo.list(string("/"));
  CPPUNIT_ASSERT_EQUAL(0UL, stored.list().size());

  repo.save(Node("/", "a", 0, 0, "abc"));
  stored = repo.list(string("/"));
  CPPUNIT_ASSERT_EQUAL(1UL, stored.list().size());
  CPPUNIT_ASSERT(repo.contains(Node("/", "a", 0, 0, "abc")));
  CPPUNIT_ASSERT(repo.contains(Node("/", "a", 0, 0, "")));
  CPPUNIT_ASSERT(!repo.contains(Node("/", "a", 0, 0, "a")));
  CPPUNIT_ASSERT(!repo.contains(Node("/", "a", 1, 0, "abc")));
  CPPUNIT_ASSERT(!repo.contains(Node("/", "a", 1, 0, "")));
  CPPUNIT_ASSERT(!repo.contains(Node("/", "b", 0, 0, "abc")));

  repo.save(Node("/", "a", 1, 0, "abc"));
  stored = repo.list(string("/"));
  CPPUNIT_ASSERT_EQUAL(1UL, stored.list().size());

  repo.save(Node("/", "b", 1, 0, "abc"));
  stored = repo.list(string("/"));
  CPPUNIT_ASSERT_EQUAL(2UL, stored.list().size());

  repo.deleted(Node("/", "b", 1, 0, ""), 2);
  stored = repo.list(string("/"));
  CPPUNIT_ASSERT_EQUAL(1UL, stored.list().size());

  repo.deleted(Node("/", "a", 1, 0, ""), 2);
  stored = repo.list(string("/"));
  CPPUNIT_ASSERT_EQUAL(0UL, stored.list().size());

  repo.save(Node("/subdir", "a", 0, 0, "abc"));
  CPPUNIT_ASSERT_EQUAL(1UL, repo.list(string("/subdir")).list().size());

  repo.deleted(Node("/subdir", "a", 0, 0, ""), 1);
  CPPUNIT_ASSERT_EQUAL(0UL, repo.list(string("/subdir")).list().size());

  CPPUNIT_ASSERT_EQUAL(0UL, repo.list(string("")).list().size());

  repo.save(Node("", "subdir", true));
  stored = repo.list(string(""));
  CPPUNIT_ASSERT_EQUAL(1UL, stored.list().size());
  Node n = stored.list().front();
  CPPUNIT_ASSERT_EQUAL(true, n.is_dir());
}

void TextNodeRepoTest::testAddRemove() {
  TextNodeRepo repo("test.db");
  auto stored = repo.list(string(""));
  CPPUNIT_ASSERT_EQUAL(0UL, stored.list().size());

  repo.save(Node("", "a", 0, 0, "abc"));
  stored = repo.list(string(""));
  CPPUNIT_ASSERT_EQUAL(1UL, stored.list().size());

  repo.deleted(Node("", "a", 1, 0, ""), 2);
  stored = repo.list(string(""));
  CPPUNIT_ASSERT_EQUAL(0UL, stored.list().size());
}

void TextNodeRepoTest::testList() {
  {
    TextNodeRepo repo("test.db");
    repo.save(Node("", "a", 1, 0, "abc"));
    repo.save(Node("", "b", 2, 0, "abc"));
    repo.save(Node("", "c", 3, 0, "abc"));

    auto stored = repo.list(string(""), 0);
    CPPUNIT_ASSERT_EQUAL(0UL, stored.list().size());

    stored = repo.list(string(""), 1);
    CPPUNIT_ASSERT_EQUAL(1UL, stored.list().size());

    stored = repo.list(string(""), 2);
    CPPUNIT_ASSERT_EQUAL(2UL, stored.list().size());

    stored = repo.list(string(""), 3);
    CPPUNIT_ASSERT_EQUAL(3UL, stored.list().size());

    stored = repo.list(string(""), 4);
    CPPUNIT_ASSERT_EQUAL(3UL, stored.list().size());
  }
  {
    TextNodeRepo repo("test.db");
    repo.save(Node("", "c", 3, 0, "abc"));
    repo.save(Node("", "b", 2, 0, "abc"));
    repo.save(Node("", "a", 1, 0, "abc"));

    auto stored = repo.list(string(""), 0);
    CPPUNIT_ASSERT_EQUAL(0UL, stored.list().size());

    stored = repo.list(string(""), 1);
    CPPUNIT_ASSERT_EQUAL(1UL, stored.list().size());

    stored = repo.list(string(""), 2);
    CPPUNIT_ASSERT_EQUAL(2UL, stored.list().size());

    stored = repo.list(string(""), 3);
    CPPUNIT_ASSERT_EQUAL(3UL, stored.list().size());

    stored = repo.list(string(""), 4);
    CPPUNIT_ASSERT_EQUAL(3UL, stored.list().size());
  }
}

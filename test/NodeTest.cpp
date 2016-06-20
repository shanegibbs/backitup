#include <string.h>

#include <cppunit/config/SourcePrefix.h>
#include <Node.h>

#include "NodeTest.h"

using namespace std;
using namespace backitup;

CPPUNIT_TEST_SUITE_REGISTRATION(NodeTest);

void NodeTest::setUp() {}

void NodeTest::tearDown() {}

void NodeTest::testGetFullPath() {
  auto root = Node::createRoot();
  CPPUNIT_ASSERT(root);
  auto file = Node::create(0, "first", root);
  CPPUNIT_ASSERT(file);

  CPPUNIT_ASSERT_EQUAL(string("/first"), *file->getFullPath());
}

void NodeTest::testGetFullPathNested() {
  auto root = Node::createRoot();
  CPPUNIT_ASSERT(root);
  auto first = Node::create(0, "first", root);
  CPPUNIT_ASSERT(first);
  auto second = Node::create(0, "second", first);
  CPPUNIT_ASSERT(second);

  CPPUNIT_ASSERT_EQUAL(string("/first/second"), *second->getFullPath());
}

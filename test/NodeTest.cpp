#include <string.h>

#include <Node.h>
#include <cppunit/config/SourcePrefix.h>

#include "NodeTest.h"

using namespace std;
using namespace backitup;

CPPUNIT_TEST_SUITE_REGISTRATION(NodeTest);

void NodeTest::setUp() {}

void NodeTest::tearDown() {}

void NodeTest::testGetFullPath() {
  {
    Node n("", "file1", 0, 0, "");
    CPPUNIT_ASSERT_EQUAL(string("file1"), n.full_path());
  }
  {
    Node n("subdir", "file1", 0, 0, "");
    CPPUNIT_ASSERT_EQUAL(string("subdir/file1"), n.full_path());
  }
  {
    Node n("subdir/nother", "file1", 0, 0, "");
    CPPUNIT_ASSERT_EQUAL(string("subdir/nother/file1"), n.full_path());
  }
}

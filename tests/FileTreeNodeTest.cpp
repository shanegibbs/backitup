#include <cppunit/config/SourcePrefix.h>
#include "FileTreeNodeTest.h"

#include <string.h>

#include <FileTreeNode.h>

using namespace std;
using namespace backitup;

CPPUNIT_TEST_SUITE_REGISTRATION(FileTreeNodeTest);

void FileTreeNodeTest::setUp() {
}

void FileTreeNodeTest::tearDown() {
}

void FileTreeNodeTest::testGetFullPath() {

  auto root = FileTreeNode::createRoot();
  auto file = FileTreeNode::create(0, "first", root);

  CPPUNIT_ASSERT_EQUAL(string("/first"), *file->getFullPath());
}

void FileTreeNodeTest::testGetFullPathNested() {

  auto root = FileTreeNode::createRoot();
  auto first = FileTreeNode::create(0, "first", root);
  auto second = FileTreeNode::create(0, "second", first);

  CPPUNIT_ASSERT_EQUAL(string("/first/second"), *second->getFullPath());
}

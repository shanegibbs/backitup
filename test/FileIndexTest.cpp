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
#include <FileIndex.h>

#include "FileIndexTest.h"
#include <Node.h>

using namespace std;
using namespace backitup;

// CPPUNIT_TEST_SUITE_REGISTRATION(FileIndexTest);

void FileIndexTest::setUp() {
  char cwd[FILENAME_MAX];
  getcwd(cwd, sizeof(cwd));
  // printf("The current working directory is %s", cwd);
  remove("index.db");
}

void FileIndexTest::tearDown() { remove("index.db"); }

void FileIndexTest::testOpenClose() { FileIndex f("test"); }

void FileIndexTest::testGetNode() {
  FileIndex f("test");
  auto node = f.getNode((unsigned int)0, "root");
  CPPUNIT_ASSERT(node);
  CPPUNIT_ASSERT_EQUAL((unsigned int)0, node->getId());
}

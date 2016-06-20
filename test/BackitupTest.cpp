#include "BackitupTest.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <cppunit/config/SourcePrefix.h>
#include DB_CXX_HEADER

#include <Backitup.h>
#include <LocalStorage.h>
#include <TextNodeRepo.h>

using namespace backitup;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(BackitupTest);

void BackitupTest::setUp() {}

void BackitupTest::tearDown() {}

void BackitupTest::testInsert() {
  auto store = LocalStorage::create("storage");
  TextNodeRepo repo;
  Backitup backitup(repo, *store);
}

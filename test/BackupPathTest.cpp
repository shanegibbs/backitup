#include "BackitupTest.h"

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <cppunit/config/SourcePrefix.h>
#include DB_CXX_HEADER

#include <boost/filesystem.hpp>
#include <iostream>

#include <BackupPath.h>

#include "BackupPathTest.h"

using namespace backitup;
using namespace std;
using namespace std::chrono_literals;

namespace fs = boost::filesystem;

CPPUNIT_TEST_SUITE_REGISTRATION(BackupPathTest);

void BackupPathTest::setUp() { fs::remove_all("backuppath_test"); }

void BackupPathTest::tearDown() {
  // fs::remove_all("backuppath_test");
}

static void create_file(const char* filename, const char* content) {
  ofstream a(filename, ios::out | ios::trunc);
  a << content;
  a.close();
}

void BackupPathTest::testVisit() {
  fs::create_directory("backuppath_test");
  fs::create_directory("backuppath_test/subdir");

  create_file("backuppath_test/a", "abc\n");
  create_file("backuppath_test/b", "defg\n");
  create_file("backuppath_test/subdir/c", "abc\n");

  vector<string> excludes;
  BackupPath backup_path("backuppath_test", excludes);

  int count = 0;
  bool root_check = false;
  bool subdir_check = false;

  backup_path.visit([&](const string& path, const NodeList& nl) -> void {
    count += 1;
    CPPUNIT_ASSERT_EQUAL(
        boost::filesystem::canonical("backuppath_test").native(), path);
    if (nl.path() == "") {
      CPPUNIT_ASSERT_EQUAL(2UL, nl.list().size());

      const Node& a = nl.list()[0];
      CPPUNIT_ASSERT_EQUAL(string("a"), a.getName());
      CPPUNIT_ASSERT_EQUAL(string(""), a.path());
      CPPUNIT_ASSERT_EQUAL(4L, a.size());

      const Node& b = nl.list()[1];
      CPPUNIT_ASSERT_EQUAL(string("b"), b.getName());
      CPPUNIT_ASSERT_EQUAL(string(""), b.path());
      CPPUNIT_ASSERT_EQUAL(5L, b.size());
      root_check = true;

    } else if (nl.path() == "subdir") {
      CPPUNIT_ASSERT_EQUAL(1UL, nl.list().size());
      const Node& a = nl.list()[0];
      CPPUNIT_ASSERT_EQUAL(string("c"), a.getName());
      CPPUNIT_ASSERT_EQUAL(string("subdir"), a.path());
      CPPUNIT_ASSERT_EQUAL(4L, a.size());
      subdir_check = true;

    } else {
      CPPUNIT_FAIL("Unexpected path: " + nl.path());
    }

  });

  CPPUNIT_ASSERT(root_check);
  CPPUNIT_ASSERT(subdir_check);
  CPPUNIT_ASSERT_EQUAL(2, count);
}

void BackupPathTest::testWatch() {
  fs::create_directory("backuppath_test");
  fs::create_directory("backuppath_test/subdir");

  vector<string> excludes;
  BackupPath backup_path("backuppath_test", excludes);

  int count = 0;
  string last_failure_msg = "";

  mutex m;
  condition_variable cv;

  backup_path.watch([&](const string& changed) -> void {
    if (changed != "" && changed != "subdir") {
      last_failure_msg = "Unexpected watch path: " + changed;
    }

    unique_lock<mutex> lk(m);
    count += 1;
    cv.notify_one();
  });

  create_file("backuppath_test/a", "abc\n");
  create_file("backuppath_test/subdir/c", "abc\n");

  unique_lock<mutex> lk(m);
  cv.wait_for(lk, 5s, [&] { return count >= 2; });

  CPPUNIT_ASSERT_EQUAL(string(""), last_failure_msg);
  CPPUNIT_ASSERT_EQUAL(2, count);
}
#include "BackitupTest.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <cppunit/config/SourcePrefix.h>
#include DB_CXX_HEADER

#include <boost/filesystem.hpp>
#include <iostream>

#include <Backitup.h>
#include <LocalStorage.h>
#include <Log.h>
#include <TextNodeRepo.h>

using namespace backitup;
using namespace std;
using namespace std::chrono_literals;

namespace fs = boost::filesystem;

CPPUNIT_TEST_SUITE_REGISTRATION(BackitupTest);

static Log LOG = Log("BackitupTest");

string raw(const char* r) {
  stringstream in(r);
  stringstream ss;

  string line;
  while (getline(in, line, '\n')) {
    line.erase(line.begin(), find_if(line.begin(), line.end(),
                                     not1(ptr_fun<int, int>(isspace))));
    if (!line.empty()) ss << line << endl;
  }

  return ss.str();
}

static void create_file(const char* filename, const char* content) {
  ofstream a(filename, ios::out | ios::trunc);
  a << content;
  a.close();
}

static void delete_file(const char* filename) {
  if (std::remove(filename) != 0) {
    throw "Failed to delete_file";
  }
}

void BackitupTest::setUp() {
  fs::remove_all("scratch.txt.db");
  fs::remove_all("backupitup_test");
}

void BackitupTest::tearDown() { /* fs::remove_all("backupitup_test"); */
}

void BackitupTest::testMain() {
  fs::create_directory("backupitup_test");
  fs::create_directory("backupitup_test/files");
  fs::create_directory("backupitup_test/files/subdir");

  create_file("backupitup_test/files/initial", "abc\n");
  create_file("backupitup_test/files/subdir/subdirA", "abc\n");
  create_file("backupitup_test/files/subdir/subdirB", "abc\n");

  BackupPath fs("backupitup_test/files", vector<string>());
  LocalStorage store("backupitup_test/storage");
  TextNodeRepo repo;

  Backitup backitup(repo, store);
  backitup.sleep_on_empty(false);

  // do initial scan and setup watches

  // auto d = ShowDebug();
  backitup.init(fs);

  auto nl = repo.latest(string(""));
  debug << "nl.dump()\n" << nl.dump();
  CPPUNIT_ASSERT_EQUAL(string(""), nl.path());
  CPPUNIT_ASSERT_EQUAL(2UL, nl.list().size());
  Node n = nl.list()[1];
  CPPUNIT_ASSERT_EQUAL(string("initial"), n.name());
  CPPUNIT_ASSERT_EQUAL(4L, n.size());
  CPPUNIT_ASSERT_EQUAL(
      string(
          "edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb"),
      n.sha256());

  string expected = raw(R"(
    . initial 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
    . subdir 0 x
    .subdir subdirA 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
    .subdir subdirB 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
  )");
  CPPUNIT_ASSERT_EQUAL(expected, repo.dump());

  mutex m;
  condition_variable cv;

  {  // create a new file

    create_file("backupitup_test/files/subdir/subdirC", "abc\n");

    int count = 0;

    auto t = backitup.run(fs, [&](const string& path) -> void {
      if (path == "subdir") {
        unique_lock<mutex> lk(m);
        count += 1;
        cv.notify_one();
      }
    });

    unique_lock<mutex> lk(m);
    cv.wait_for(lk, 5s, [&] { return count >= 1; });
    CPPUNIT_ASSERT_EQUAL(1, count);

    backitup.stop();
    t.join();

    string expected = raw(R"(
      . initial 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      . subdir 0 x
      .subdir subdirA 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      .subdir subdirB 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      .subdir subdirC 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
    )");
    CPPUNIT_ASSERT_EQUAL(expected, repo.dump());
  }

  {  // delete file

    delete_file("backupitup_test/files/subdir/subdirC");

    int count = 0;

    auto t = backitup.run(fs, [&](const string& path) -> void {
      if (path == "subdir") {
        unique_lock<mutex> lk(m);
        count += 1;
        cv.notify_one();
      }
    });

    unique_lock<mutex> lk(m);
    cv.wait_for(lk, 5s, [&] { return count >= 1; });
    CPPUNIT_ASSERT_EQUAL(1, count);

    backitup.stop();
    t.join();

    string expected = raw(R"(
      . initial 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      . subdir 0 x
      .subdir subdirA 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      .subdir subdirB 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      .subdir subdirC 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      .subdir subdirC 4 _
    )");
    CPPUNIT_ASSERT_EQUAL(expected, repo.dump());
  }

  {  // create a new file in new directory

    fs::create_directory("backupitup_test/files/dirtest");
    create_file("backupitup_test/files/dirtest/dirtestfile", "abc\n");

    int count = 0;

    auto t = backitup.run(fs, [&](const string& path) -> void {
      if (path == "dirtest") {
        unique_lock<mutex> lk(m);
        count += 1;
        cv.notify_one();
      }
    });

    unique_lock<mutex> lk(m);
    cv.wait_for(lk, 5s, [&] { return count >= 1; });
    CPPUNIT_ASSERT_EQUAL(1, count);

    backitup.stop();
    t.join();

    string expected = raw(R"(
      . dirtest 0 x
      . initial 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      . subdir 0 x
      .dirtest dirtestfile 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      .subdir subdirA 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      .subdir subdirB 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      .subdir subdirC 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      .subdir subdirC 4 _
    )");
    CPPUNIT_ASSERT_EQUAL(expected, repo.dump());
  }

  {  // delete directory

    fs::remove_all("backupitup_test/files/dirtest");

    int count = 0;

    auto t = backitup.run(fs, [&](const string& path) -> void {
      if (path == "dirtest") {
        unique_lock<mutex> lk(m);
        count += 1;
        cv.notify_one();
      }
    });

    unique_lock<mutex> lk(m);
    cv.wait_for(lk, 5s, [&] { return count >= 1; });
    CPPUNIT_ASSERT_EQUAL(1, count);

    backitup.stop();
    t.join();

    string expected = raw(R"(
      . dirtest 0 x
      . dirtest 0 _
      . initial 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      . subdir 0 x
      .dirtest dirtestfile 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      .dirtest dirtestfile 4 _
      .subdir subdirA 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      .subdir subdirB 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      .subdir subdirC 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
      .subdir subdirC 4 _
    )");
    CPPUNIT_ASSERT_EQUAL("\n" + expected, "\n" + repo.dump());
  }

  fs.stop();
}

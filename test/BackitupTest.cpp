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
#include <TextNodeRepo.h>

using namespace backitup;
using namespace std;
using namespace std::chrono_literals;

namespace fs = boost::filesystem;

CPPUNIT_TEST_SUITE_REGISTRATION(BackitupTest);

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

  backitup.init(fs);

  auto nl = repo.latest(string("/"));
  cout << nl.dump() << endl;
  CPPUNIT_ASSERT_EQUAL(string("/"), nl.path());
  CPPUNIT_ASSERT_EQUAL(1UL, nl.list().size());
  Node n = nl.list()[0];
  CPPUNIT_ASSERT_EQUAL(string("initial"), n.getName());
  CPPUNIT_ASSERT_EQUAL(4L, n.size());
  CPPUNIT_ASSERT_EQUAL(
      string(
          "edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb"),
      n.sha256());

  string expected = raw(R"(
    / initial 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
    subdir subdirA 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
    subdir subdirB 4 edeaaff3f1774ad2888673770c6d64097e391bc362d7d6fb34982ddf0efd18cb
  )");
  CPPUNIT_ASSERT_EQUAL(expected, repo.dump());

  // make a change for the watcher to pick up

  create_file("backupitup_test/files/subdir/subdirC", "abc\n");

  int count = 0;

  mutex m;
  condition_variable cv;

  backitup.run(fs, [&](const string& path) -> void {
    cerr << "GOOD " << path << endl;
    if (path == "subdir") {
      unique_lock<mutex> lk(m);
      count += 1;
      cv.notify_one();
    }
  });

  unique_lock<mutex> lk(m);
  cv.wait_for(lk, 5s, [&] { return count >= 1; });
  cerr << "TESTCOMPLETE" << endl;

  backitup.stop();
  cerr << "QUIT" << endl;
}

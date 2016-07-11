#include <string>

#include "OptionsTest.h"

#include <Log.h>
#include <Options.h>

using namespace backitup;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(OptionsTest);

static Log LOG = Log("OptionsTest");

void OptionsTest::setUp() {}

void OptionsTest::tearDown() {}

void OptionsTest::test_parse_max_file_size_bytes() {
  Options options;
  CPPUNIT_ASSERT_EQUAL(12UL, options.parse_max_file_size_bytes("12B"));
  CPPUNIT_ASSERT_EQUAL(13312UL, options.parse_max_file_size_bytes("13KB"));
  CPPUNIT_ASSERT_EQUAL(14680064UL, options.parse_max_file_size_bytes("14MB"));
  CPPUNIT_ASSERT_EQUAL(16106127360UL,
                       options.parse_max_file_size_bytes("15GB"));
  CPPUNIT_ASSERT_THROW(options.parse_max_file_size_bytes("1"),
                       OptionsException);
}

void OptionsTest::test_parse_path_time_spec() {
  Options options;
  auto val = options.parse_path_time_spec("/home/sgibbs/Documents",
                                          "/home/sgibbs", "");
  CPPUNIT_ASSERT_EQUAL(string("Documents"), val.first);
  CPPUNIT_ASSERT(val.second >= 1468199691);
  CPPUNIT_ASSERT(val.second < 2000000000);
}

void OptionsTest::test_parse_interval() {
  Options options;

  auto val = options.parse_interval("1s");
  CPPUNIT_ASSERT_EQUAL(string("1s"), val.first);
  CPPUNIT_ASSERT_EQUAL(1, val.second);

  val = options.parse_interval("1m");
  CPPUNIT_ASSERT_EQUAL(60, val.second);

  val = options.parse_interval("2m");
  CPPUNIT_ASSERT_EQUAL(120, val.second);

  val = options.parse_interval("2h");
  CPPUNIT_ASSERT_EQUAL(7200, val.second);
}

#ifndef OPTIONSTEST_H_
#define OPTIONSTEST_H_

#include <cppunit/extensions/HelperMacros.h>

class OptionsTest : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(OptionsTest);
  CPPUNIT_TEST(test_parse_max_file_size_bytes);
  CPPUNIT_TEST(test_parse_path_time_spec);
  CPPUNIT_TEST_SUITE_END();

 public:
  void setUp();
  void tearDown();
  void test_parse_max_file_size_bytes();
  void test_parse_path_time_spec();
};

#endif /* BACKITUPTEST_H_ */

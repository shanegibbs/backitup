#ifndef BACKITUPTEST_H_
#define BACKITUPTEST_H_

#include <cppunit/extensions/HelperMacros.h>

class BackitupTest : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(BackitupTest);
  CPPUNIT_TEST(testMain);
  CPPUNIT_TEST_SUITE_END();

 public:
  void setUp();
  void tearDown();
  void testMain();
};

#endif /* BACKITUPTEST_H_ */

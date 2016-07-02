#ifndef BACKUPPATHTEST_H_
#define BACKUPPATHTEST_H_

#include <cppunit/extensions/HelperMacros.h>

class BackupPathTest : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(BackupPathTest);
  CPPUNIT_TEST(testVisit);
  CPPUNIT_TEST(testWatch);
  CPPUNIT_TEST_SUITE_END();

 public:
  void setUp();
  void tearDown();
  void testVisit();
  void testWatch();
  void testExcludes();
};

#endif /* BACKUPPATHTEST_H_ */

#ifndef NODEREPOTEST_H_
#define NODEREPOTEST_H_

#include <cppunit/extensions/HelperMacros.h>

class NodeRepoTest : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(NodeRepoTest);
  CPPUNIT_TEST(testSaveRoot);
  CPPUNIT_TEST(testSave);
  CPPUNIT_TEST_SUITE_END();

 protected:

 public:
  void setUp();
  void tearDown();
  void testSaveRoot();
  void testSave();
};

#endif /* NODEREPOTEST_H_ */

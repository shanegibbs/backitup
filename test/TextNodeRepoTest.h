#ifndef TEXTNODEREPOTEST_H_
#define TEXTNODEREPOTEST_H_

#include <cppunit/extensions/HelperMacros.h>

#include <TextNodeRepo.h>

class TextNodeRepoTest : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(TextNodeRepoTest);
  CPPUNIT_TEST(testMain);
  CPPUNIT_TEST(testAddRemove);
  CPPUNIT_TEST(testList);
  CPPUNIT_TEST_SUITE_END();

 public:
  void setUp();
  void tearDown();
  void testMain();
  void testAddRemove();
  void testList();
};

#endif /* REPOSITORYTEST_H_ */

/*
 * DataHelperFnTest.h
 *
 *  Created on: Jul 20, 2015
 *      Author: sgibbs
 */

#ifndef DATA_HELPER_FN_TEST_H_
#define DATA_HELPER_FN_TEST_H_

#include <cppunit/extensions/HelperMacros.h>

class DataHelperFnTest : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(DataHelperFnTest);
  CPPUNIT_TEST(testParse);
  CPPUNIT_TEST_SUITE_END();

 protected:

 public:
  void setUp();
  void tearDown();
  void testParse();
};

#endif /* DATA_HELPER_FN_TEST_H_ */


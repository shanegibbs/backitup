/*
 * FileIndexTest.h
 *
 *  Created on: Jul 20, 2015
 *      Author: sgibbs
 */

#ifndef FILEINDEXTEST_H_
#define FILEINDEXTEST_H_

#include <cppunit/extensions/HelperMacros.h>

class FileIndexTest : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(FileIndexTest);
  CPPUNIT_TEST(testOpenClose);
  CPPUNIT_TEST(testGetNode);
  CPPUNIT_TEST_SUITE_END();

 protected:
  double m_value1;
  double m_value2;

 public:
  void setUp();
  void tearDown();
  void testOpenClose();
  void testGetNode();
};

#endif /* FILEINDEXTEST_H_ */

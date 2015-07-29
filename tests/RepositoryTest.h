/*
 * RepositoryTest.h
 *
 *  Created on: Jul 22, 2015
 *      Author: sgibbs
 */

#ifndef REPOSITORYTEST_H_
#define REPOSITORYTEST_H_

#include <cppunit/extensions/HelperMacros.h>

#include <Database.h>

using namespace std;
using namespace backitup;

class RepositoryTest : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(RepositoryTest);
  CPPUNIT_TEST(testInsert);
  CPPUNIT_TEST(testGet);
  CPPUNIT_TEST(testGetNotFound);
  CPPUNIT_TEST_SUITE_END();

 protected:
  shared_ptr<Database> db;

 public:
  void setUp();
  void tearDown();
  void testInsert();
  void testGet();
  void testGetNotFound();
};

#endif /* REPOSITORYTEST_H_ */

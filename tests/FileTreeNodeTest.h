#ifndef FILETREENODETEST_H_
#define FILETREENODETEST_H_

#include <cppunit/extensions/HelperMacros.h>

class FileTreeNodeTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( FileTreeNodeTest );
  CPPUNIT_TEST( testGetFullPath );
  CPPUNIT_TEST( testGetFullPathNested );
  CPPUNIT_TEST_SUITE_END();

protected:
  double m_value1;
  double m_value2;

public:
  void setUp();
  void tearDown();
  void testGetFullPath();
  void testGetFullPathNested();

};

#endif /* FILETREENODETEST_H_ */

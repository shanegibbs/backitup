TESTS = backitup_test
check_PROGRAMS = $(TESTS)

backitup_test_SOURCES = main.cpp \
  FileTreeNodeTest.cpp

backitup_test_CXXFLAGS = @cppunit_CFLAGS@ -I$(top_srcdir)/src
backitup_test_LDFLAGS = @cppunit_LIBS@
backitup_test_LDADD = $(top_builddir)/src/libbackitup.la

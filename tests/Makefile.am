bin_PROGRAMS = main

main_SOURCES = main.cpp \
  FileTreeNodeTest.cpp

main_CXXFLAGS = @cppunit_CFLAGS@ -I$(top_srcdir)/src
main_LDFLAGS = @cppunit_LIBS@
main_LDADD = $(top_builddir)/src/libbackitup.la

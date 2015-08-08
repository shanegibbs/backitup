noinst_LTLIBRARIES = libbackitup.la
#noinst_LIBRARIES = libbackitup.a

BUILT_SOURCES = Database.pb.cc

Database.pb.cc: Database.proto
	protoc -I=$(srcdir) --cpp_out=$(builddir) $(srcdir)/Database.proto

libbackitup_la_SOURCES = \
  BackupPath.cpp \
  Node.cpp \
  FileIndex.cpp \
	Database.cpp \
  Database.pb.cc \
	Repository.cpp \
	RepositoryIndex.cpp \
	NodeRepo.cpp

libbackitup_la_CFLAGS = @protobuf_CFLAGS@

libbackitup_la_LDFLAGS = -module -avoid-version \
  @DB_CXX_LIBS@ \
	@protobuf_LIBS@

bin_PROGRAMS = backitup
backitup_SOURCES = main.cpp
backitup_LDADD = libbackitup.la

CLEANFILES = Database.pb.cc Database.pb.h

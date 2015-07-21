bin_PROGRAMS = backitup
backitup_SOURCES = main.cpp
backitup_LDADD = libbackitup.la

noinst_LTLIBRARIES = libbackitup.la
libbackitup_la_SOURCES = BackupPath.cpp FileTreeNode.cpp FileIndex.cpp
libbackitup_la_LDFLAGS = -module -avoid-version @DB_CXX_LIBS@

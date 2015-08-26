AC_INIT([backitup], [0.1])
AC_CONFIG_SRCDIR([src/main.cpp])
#AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([serial-tests])
AM_SILENT_RULES([yes])

AC_PROG_CXX
AC_LANG([C++])

LT_INIT([shared])

AX_CXX_COMPILE_STDCXX_11([],[mandatory])

AX_BERKELEY_DB_CXX([5], [], [
  AC_MSG_ERROR([Error, berkeley db not found])
])

PKG_CHECK_MODULES([protobuf], [protobuf], [], [
  AC_MSG_ERROR([protobuf not found])
])

PKG_CHECK_MODULES([cppunit], [cppunit], [], [
  AC_MSG_ERROR([cppunit not found])
])

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    tests/Makefile
    ])
AC_OUTPUT

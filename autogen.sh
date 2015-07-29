#!/bin/sh
# autoreconf --force --install

case `uname` in Darwin*) glibtoolize --copy ;;
    *) libtoolize --copy ;; esac

autoheader
aclocal -I m4 --install
autoconf

automake --foreign --add-missing --force-missing --copy

#!/bin/sh
# autoreconf --force --install

autoheader
aclocal -I m4 --install
autoconf

automake --foreign --add-missing --force-missing --copy

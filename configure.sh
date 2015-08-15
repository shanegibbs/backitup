#!/bin/sh

../configure CXX=clang++ CXXFLAGS='-g -O0 -std=c++11 -Wall -Werror -fsanitize=address -fno-omit-frame-pointer -fcolor-diagnostics' --enable-silent-rules

#!/bin/sh

../configure CXX=clang++-3.6 CXXFLAGS='-g -O0 -std=c++11 -Wall -Werror -fsanitize=address -fno-omit-frame-pointer' --enable-silent-rules

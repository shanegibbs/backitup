cmake_minimum_required(VERSION 3.2)

project(backitup VERSION 0.0.1)

set(CMAKE_CXX_COMPILER "/Users/sgibbs/usr/bin/clang++")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -fvisibility=hidden")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer -g -gstabs")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -ggdb")

link_directories(/usr/local/lib)

# boost

set(BOOST_ROOT /usr/lib/sc-boost-1.59.0)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.59.0 COMPONENTS log_setup log thread system filesystem program_options regex REQUIRED)
message("-- Found Boost include dirs ${Boost_INCLUDE_DIRS}")

# db

add_definitions(-DDB_CXX_HEADER="db_cxx.h")

# rapidjson

#add_definitions(-DRAPIDJSON_HAS_STDSTRING)
#include_directories("deps/rapidjson/include")

# mustache

#add_subdirectory(deps/mstch)
#include_directories(${mstch_INCLUDE_DIR})

# main

string(TIMESTAMP BUILD_TS "%Y%m%dT%H%M%SZ" UTC)

set(PACKAGE_VERSION "${PROJECT_VERSION}")
if (NOT $ENV{PACKAGE_SUFFIX} STREQUAL "")
  set(PACKAGE_VERSION "${PROJECT_VERSION}-${BUILD_TS}")
endif (NOT $ENV{PACKAGE_SUFFIX} STREQUAL "")

add_definitions(-DVERSION="${PACKAGE_VERSION}")
add_definitions(-DBUILD_TS="${BUILD_TS}")

exec_program(git ${CMAKE_SOURCE_DIR} ARGS "rev-parse HEAD" OUTPUT_VARIABLE GIT_SHA1)
add_definitions(-DGIT_SHA1="${GIT_SHA1}")

find_package(PkgConfig REQUIRED)
#pkg_search_module(Curl REQUIRED sc-libcurl)
#include_directories(${Curl_INCLUDE_DIRS})
#message("-- Found sc-libcurl include dirs ${Curl_INCLUDE_DIRS}")

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
else(Boost_FOUND)
  message(FATAL_ERROR "Boost not found.")
endif(Boost_FOUND)

add_subdirectory(src)

#enable_testing()
#add_subdirectory(tests)

add_custom_target(scratch
  COMMAND mkdir -p scratch/subdir
	COMMAND touch scratch/file-a scratch/subdir/file-{a,b,c}
	COMMAND echo 0 > scratch/zero
	COMMAND echo 1 > scratch/one
  )

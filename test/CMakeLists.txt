include_directories (
  ${CMAKE_SOURCE_DIR}/src/lib
  ${Boost_INCLUDE_DIRS}
  /usr/local/Cellar/cppunit/1.13.2/include
)

message("-- Tests using include dirs ${CMAKE_SOURCE_DIR}/src")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreServices")

link_directories(/usr/local/Cellar/cppunit/1.13.2/lib)

add_definitions(-DBOOST_TEST_DYN_LINK)

file(GLOB test_SRC
  "*.h"
  "*.cpp"
)
# ${test_SRC}

add_executable(backitup_unittests main.cpp
  TextNodeRepoTest.cpp
  BackitupTest.cpp
)

target_link_libraries (backitup_unittests
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  cppunit
  backitup
  crypto
  ssl.a
  z.a
)

add_test(NAME backitup_unittests COMMAND backitup_unittests)
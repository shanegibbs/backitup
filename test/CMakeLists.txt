include_directories (
  ${CMAKE_SOURCE_DIR}/src/lib
  ${Boost_INCLUDE_DIRS}
  /usr/local/Cellar/cppunit/1.13.2/include
)

message("-- Tests using include dirs ${CMAKE_SOURCE_DIR}/src")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreServices")

link_directories(/usr/local/Cellar/cppunit/1.13.2/lib)

link_libraries(${Boost_LIBRARIES})

add_definitions(-DBOOST_TEST_DYN_LINK)

file(GLOB test_SRC
  "*.h"
  "*.cpp"
)
# ${test_SRC}

add_executable(backitup_unittests main.cpp
  TextNodeRepoTest.cpp
  BackupPathTest.cpp
  BackitupTest.cpp
  NodeTest.cpp
)

target_link_libraries (backitup_unittests
  cppunit
  backitup
  crypto
  ssl.a
  bz2.a
  z.a
)

add_test(NAME backitup_unittests COMMAND backitup_unittests)

add_test(NAME IT_tests COMMAND ${CMAKE_SOURCE_DIR}/test/scripts/it.sh)

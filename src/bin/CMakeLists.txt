include_directories("${PROJECT_SOURCE_DIR}/src/lib")
include_directories("${PROJECT_BINARY_DIR}/src/lib")

add_library(backitup-cli Options.cpp)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreServices")

link_libraries(backitup)
link_libraries(backitup-cli)
link_libraries(${Boost_LIBRARIES})
link_libraries(ssl.a crypto bz2.a z.a)

add_executable(backitup-bin main.cpp)
set_target_properties(backitup-bin PROPERTIES OUTPUT_NAME backitup)
add_executable(restoreitup restoreitup.cpp)

add_executable(backuppath backuppath.cpp)

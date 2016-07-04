include_directories("${PROJECT_SOURCE_DIR}/src/lib")
include_directories("${PROJECT_BINARY_DIR}/src/lib")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreServices")

link_libraries(backitup)
link_libraries(boost_system)
link_libraries(boost_filesystem)
link_libraries(boost_program_options)
link_libraries(db_cxx-6.1)
link_libraries(protobuf)
link_libraries(ssl.a crypto z.a)

add_executable(backitup-bin main.cpp)
set_target_properties(backitup-bin PROPERTIES OUTPUT_NAME backitup)

add_executable(backuppath backuppath.cpp)

include_directories("${PROJECT_SOURCE_DIR}/src/lib")
include_directories("${PROJECT_BINARY_DIR}/src/lib")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreServices")

add_executable(backitup-bin main.cpp)
set_target_properties(backitup-bin PROPERTIES OUTPUT_NAME backitup)
target_link_libraries(backitup-bin backitup)
target_link_libraries(backitup-bin boost_system)
target_link_libraries(backitup-bin boost_filesystem)
target_link_libraries(backitup-bin db_cxx-6.1)
target_link_libraries(backitup-bin protobuf)
target_link_libraries(backitup-bin ssl.a crypto z.a)

#include_directories(${CMAKE_SOURCE_DIR}/src/lib)

add_custom_command (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Database.pb.cc
  COMMAND protoc -I=${PROJECT_SOURCE_DIR}/src --cpp_out=${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/src/Database.proto
  )

include_directories("${PROJECT_BINARY_DIR}/src")

include_directories("/usr/local/include")
include_directories("/usr/local/Cellar/openssl/1.0.2f/include")

file(GLOB lib_SRC
  "*.h"
  "*.cpp"
)

add_executable(backitup-bin ${lib_SRC} Database.pb.cc)
set_target_properties(backitup-bin PROPERTIES OUTPUT_NAME backitup)
target_link_libraries(backitup-bin boost_system)
target_link_libraries(backitup-bin boost_filesystem)
target_link_libraries(backitup-bin db_cxx-6.1)
target_link_libraries(backitup-bin protobuf)
target_link_libraries(backitup-bin ssl.a crypto z.a)

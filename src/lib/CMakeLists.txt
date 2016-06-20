add_custom_command (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Database.pb.cc
  COMMAND protoc -I=${PROJECT_SOURCE_DIR}/src/lib --cpp_out=${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/src/lib/Database.proto
  )

include_directories("${PROJECT_BINARY_DIR}/src/lib")
include_directories("/usr/local/include")
include_directories("/usr/local/Cellar/openssl/1.0.2f/include")

file(GLOB lib_SRC
  "*.h"
  "*.cpp"
)

add_library(backitup ${lib_SRC} Database.pb.cc)

#target_link_libraries(backitup-bin boost_system)
#target_link_libraries(backitup-bin boost_filesystem)
#target_link_libraries(backitup-bin db_cxx-6.1)
#target_link_libraries(backitup-bin protobuf)
#target_link_libraries(backitup-bin ssl.a crypto z.a)

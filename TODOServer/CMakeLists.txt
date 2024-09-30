# CMakeList.txt : CMake project for TODOServer, include source and define
# project specific logic here.
#

file(GLOB SOURCES "source/*.cpp" "source/*.h")

set(MYSQL_INCLUDE_DIR "/usr/include")  # Path where MySQL headers are located
set(MYSQL_LIBRARY "/usr/lib/aarch64-linux-gnu/libmariadbclient.so")  # MySQL client library
set(MYSQLCPP_INCLUDE_DIR "/usr/include/cppconn")  # Path to MySQL Connector/C++ headers
set(MYSQLCPP_LIBRARY "/usr/lib/aarch64-linux-gnu/libmysqlcppconn.so")  # MySQL Connector/C++ library

include_directories(${MYSQL_INCLUDE_DIR} ${MYSQLCPP_INCLUDE_DIR})

# Add source to this project's executable.
add_executable (TODOServer ${SOURCES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TODOServer PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(TODOServer PRIVATE external/crow)
target_include_directories(TODOServer PRIVATE ../../TODOShared)

target_link_libraries(TODOServer PRIVATE ${MYSQL_LIBRARY} ${MYSQLCPP_LIBRARY})

# TODO: Add tests and install targets if needed.

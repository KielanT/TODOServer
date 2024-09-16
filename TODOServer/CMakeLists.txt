# CMakeList.txt : CMake project for TODOServer, include source and define
# project specific logic here.
#

file(GLOB SOURCES "source/*.cpp" "source/*.h")

# Add source to this project's executable.
add_executable (TODOServer ${SOURCES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TODOServer PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(TODOServer PRIVATE external/crow)
target_include_directories(TODOServer PRIVATE ../../TODOShared)


# TODO: Add tests and install targets if needed.

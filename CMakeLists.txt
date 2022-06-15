project("Light Source Engine" C)
cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(config/cmake/functions.cmake)

add_subdirectory(external)
add_subdirectory(lib)
add_subdirectory(bindings/js/lse/core/addon)

if (BUILD_TESTING)
  add_subdirectory(test)
endif()

# TODO: add_subdirectory(apps)

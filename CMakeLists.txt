project(LightSourceEngine C)
cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

include(config/cmake/functions.cmake)

add_subdirectory(external)

add_subdirectory(lib)
add_subdirectory(test)

# TODO: add_subdirectory(apps)
# TODO: add_subdirectory(js/lse/core/addon)

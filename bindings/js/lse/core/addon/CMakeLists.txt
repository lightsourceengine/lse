cmake_minimum_required(VERSION 3.20)
project(lse-core CXX)

set(CMAKE_CXX_STANDARD 11)

from_build_gyp("build.gypi" LSE_CORE_SOURCES X)

include_directories(
    "src"
    "${CMAKE_SOURCE_DIR}/external/freetype/repo/include"
    "${CMAKE_SOURCE_DIR}/external/nanosvg/include"
    "${CMAKE_SOURCE_DIR}/external/nanosvg/repo-local/src"
    "${CMAKE_SOURCE_DIR}/external/SDL2/repo/include"
    "${CMAKE_SOURCE_DIR}/external/stb/repo"
    "${CMAKE_SOURCE_DIR}/external/STC/repo/include"
    "${CMAKE_SOURCE_DIR}/external/node-api-headers/repo/include"
    "${CMAKE_SOURCE_DIR}/lib/include"
    "${CMAKE_SOURCE_DIR}/lib/src"
)

add_library(
    lse-core
    SHARED
    ${LSE_CORE_SOURCES}
)

add_compile_definitions(NODE_GYP_MODULE_NAME=lse_core NAPI_VERSION=3)

set_target_properties(lse-core
    PROPERTIES
    PREFIX ""
    SUFFIX ".node"
)

target_link_libraries(
    lse-core
    lse
    nanosvg
    stc
    stb_image
    freetype
    yoga
    "-Wl,-undefined,dynamic_lookup"
)

project(lse C)

from_build_gyp("build.gypi" LSE_LIB_SOURCES X)

add_library(
    lse
    STATIC
    ${LSE_LIB_SOURCES}
)

target_include_directories(
    lse
    PUBLIC
    "include"
    PRIVATE
    "${CMAKE_SOURCE_DIR}/external/freetype/repo/include"
    "${CMAKE_SOURCE_DIR}/external/nanosvg/repo-local/src"
    "${CMAKE_SOURCE_DIR}/external/nanosvg/include"
    "${CMAKE_SOURCE_DIR}/external/SDL2/repo/include"
    "${CMAKE_SOURCE_DIR}/external/stb/repo"
    "${CMAKE_SOURCE_DIR}/external/STC/repo/include"
    "${CMAKE_SOURCE_DIR}/external/yoga/repo-local/yoga"
    src
)

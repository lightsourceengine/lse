project(nanosvg C)
set(CMAKE_C_STANDARD 99)

from_build_gyp("build.gypi" NANOSVG_SOURCES NANOSVG_INCLUDES)

add_library(
    nanosvg
    STATIC
    ${NANOSVG_SOURCES}
)

target_include_directories(
    nanosvg
    PUBLIC
    ${NANOSVG_INCLUDES}
)

install(TARGETS nanosvg)

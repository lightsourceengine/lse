project(freetype C)
set(CMAKE_C_STANDARD 99)

add_compile_definitions(
    FT2_BUILD_LIBRARY
    FT_CONFIG_OPTION_ERROR_STRINGS
    FT_USE_SMOOTH
)

from_build_gyp("build.gypi" FREETYPE_SOURCES FREETYPE_INCLUDES)

add_library(
    freetype
    STATIC
    ${FREETYPE_SOURCES}
)

target_include_directories(
    freetype
    PUBLIC
    ${FREETYPE_INCLUDES}
)

install(TARGETS freetype)

project(stb_image C)
set(CMAKE_C_STANDARD 99)

from_build_gyp("build.gypi" STB_IMAGE_SOURCES STB_IMAGE_INCLUDES)

add_library(
    stb_image
    STATIC
    ${STB_IMAGE_SOURCES}
)

target_include_directories(
    stb_image
    PUBLIC
    ${STB_IMAGE_INCLUDES}
)

install(TARGETS stb_image)

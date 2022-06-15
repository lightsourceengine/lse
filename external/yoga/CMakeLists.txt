project(yoga CXX)
set(CMAKE_CXX_STANDARD 11)

from_build_gyp("build.gypi" YOGA_SOURCES YOGA_INCLUDES)

add_library(
    yoga
    STATIC
    ${YOGA_SOURCES}
)

target_include_directories(
    yoga
    PUBLIC
    ${YOGA_INCLUDES}
)

target_compile_definitions(
    yoga
    PUBLIC
    YG_ENABLE_NODE_LAYOUT_EVENT=1
    YOGA_EXPORT=
)

install(TARGETS yoga)

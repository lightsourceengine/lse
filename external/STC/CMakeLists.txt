cmake_minimum_required(VERSION 3.20)
project(stc C)

set(CMAKE_C_STANDARD 99)

from_build_gyp("build.gypi" STC_SOURCES STC_INCLUDES)

add_library(
    stc
    STATIC
    ${STC_SOURCES}
)

target_include_directories(
    stc
    PUBLIC
    ${STC_INCLUDES}
)

install(TARGETS stc)

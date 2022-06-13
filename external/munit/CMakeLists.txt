project(munit C)
set(CMAKE_C_STANDARD 99)

add_library(
    munit
    STATIC
    repo/munit.c
)

target_include_directories(
    munit
    PUBLIC
    repo
)

install(TARGETS munit)

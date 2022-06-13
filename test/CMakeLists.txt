project(lse-test CXX)

include_directories(
    "${CMAKE_SOURCE_DIR}/external/freetype/repo/include"
    "${CMAKE_SOURCE_DIR}/external/nanosvg/repo-local/src"
    "${CMAKE_SOURCE_DIR}/external/nanosvg/include"
    "${CMAKE_SOURCE_DIR}/external/SDL2/repo/include"
    "${CMAKE_SOURCE_DIR}/external/STC/repo/include"
    "${CMAKE_SOURCE_DIR}/external/yoga/repo-local/yoga"
    "${CMAKE_SOURCE_DIR}/lib/include"
    "${CMAKE_SOURCE_DIR}/lib/src"
    "src/framework"
)

add_executable(
    lse-test
    src/runner/lse_test_runner.c
    src/runner/lse_test_runner_suite.c
    src/framework/lse_test.c
    src/test_lse_array.c
    src/test_lse_env.c
    src/test_lse_event.c
    src/test_lse_font.c
    src/test_lse_font_store.c
    src/test_lse_image.c
    src/test_lse_image_store.c
    src/test_lse_node.c
    src/test_lse_object.c
    src/test_lse_string.c
    src/test_lse_style.c
    src/test_lse_style_meta.c
    src/test_lse_text.c
    src/test_lse_window.c
)

target_link_libraries(
    lse-test
    freetype
    munit
    lse
    nanosvg
    stb_image
    stc
    yoga
)

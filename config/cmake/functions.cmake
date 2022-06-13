function(from_build_gyp BUILD_GYP_FILE SOURCE_LIST INCLUDE_LIST)
  file(READ "${BUILD_GYP_FILE}" JSON_BLOB)
  string(JSON LEN LENGTH ${JSON_BLOB} targets 0 sources)
  math(EXPR LEN "${LEN}-1")
  foreach(IDX RANGE ${LEN})
    string(JSON ENTRY GET ${JSON_BLOB} targets 0 sources ${IDX})
    list(APPEND TEMP_LIST ${ENTRY})
  endforeach()
  set("${SOURCE_LIST}" "${TEMP_LIST}" PARENT_SCOPE)
  string(JSON LEN LENGTH ${JSON_BLOB} targets 0 include_dirs)
  math(EXPR LEN "${LEN}-1")
  foreach(IDX RANGE ${LEN})
    string(JSON ENTRY GET ${JSON_BLOB} targets 0 include_dirs ${IDX})
    list(APPEND TEMP_LIST ${ENTRY})
  endforeach()
  set("${INCLUDE_LIST}" "${TEMP_LIST}" PARENT_SCOPE)
endfunction()

# TEST SETUP

set(TESTS_OUTPUT ${CMAKE_SOURCE_DIR}/tests_output)
set(COVERAGE_TARGET coverage)

add_custom_target(${COVERAGE_TARGET})

function(setup_test_target TARGET_NAME TEST_NAME COVERAGE_NAME TESTING_SOURCES)

  get_property(prj_sources GLOBAL PROPERTY PROJECT_SOURCES)
  add_executable(${TEST_NAME} ${prj_sources} ${TESTING_SOURCES})

  target_link_libraries(${TEST_NAME} gtest_main gtest gmock)

  setup_target_for_coverage(
    ${COVERAGE_NAME} ${TARGET_NAME} ${TEST_NAME} ${COVERAGE_NAME}
  )

  add_dependencies(${COVERAGE_TARGET} ${COVERAGE_NAME})
  
endfunction(setup_test_target)

# TEST DIRECTORIES
add_subdirectory(correctness)

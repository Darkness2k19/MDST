set(TARGET_NAME correctness)
set(TEST_NAME ${TARGET_NAME}_test)
set(COVERAGE_NAME ${TARGET_NAME}_coverage)

set(TESTING_SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/tests.hpp
  ${CMAKE_CURRENT_LIST_DIR}/tests.cpp
)

setup_test_target(${TARGET_NAME} ${TEST_NAME} ${COVERAGE_NAME} "${TESTING_SOURCES}")

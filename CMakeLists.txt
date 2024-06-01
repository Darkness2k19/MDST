cmake_minimum_required(VERSION 3.17)
project(main)

option(DEBUG OFF)
option(DUMMY OFF)
option(STATS OFF)
option(TESTING OFF)

define_property(GLOBAL PROPERTY PROJECT_SOURCES 
  BRIEF_DOCS "Project sources" FULL_DOCS "Project sources")
set_property(GLOBAL PROPERTY PROJECT_SOURCES "")

macro(add_sources HEADERS SOURCES)
  set_property(GLOBAL APPEND PROPERTY PROJECT_SOURCES
    ${HEADERS}
    ${SOURCES}
  )
endmacro()

if (DUMMY)
  add_compile_definitions(DUMMY)
endif()

if (STATS)
  add_compile_definitions(STATS)
endif()

if (TESTING)
  include(FetchContent)
  FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG main
  )
  FetchContent_MakeAvailable(googletest)

  add_compile_definitions(TESTING)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage --coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
  set(DEBUG ON)
endif()

if (DEBUG)
  set(CMAKE_BUILD_TYPE Debug)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
else()
  set(CMAKE_BUILD_TYPE Release)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories(src)
add_subdirectory(src)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" ${PROJECT_SOURCE_DIR}/cmake)

if (TESTING)
  include(CodeCoverage)
  add_subdirectory(tests)
endif()

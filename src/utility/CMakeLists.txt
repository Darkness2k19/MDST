set(HEADERS
  ${CMAKE_CURRENT_LIST_DIR}/graph.hpp
  ${CMAKE_CURRENT_LIST_DIR}/reader.hpp
  ${CMAKE_CURRENT_LIST_DIR}/writer.hpp
)

set(SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/graph.cpp
  ${CMAKE_CURRENT_LIST_DIR}/reader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/writer.cpp
)


add_sources("${HEADERS}" "${SOURCES}")

add_library(cli STATIC)

target_sources(cli
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/cli.h
)

target_sources(cli PRIVATE
  src/cli.cpp
)

target_include_directories(cli
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(cli PUBLIC
  coreengine
)

set_target_properties(cli PROPERTIES LINKER_LANGUAGE CXX)
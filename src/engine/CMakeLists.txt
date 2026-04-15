add_library(engine STATIC)

target_sources(engine
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/engine.h
)

target_sources(engine PRIVATE
  src/engine.cpp
)

target_include_directories(engine
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

set_target_properties(engine PROPERTIES LINKER_LANGUAGE CXX)
add_library(backend STATIC)

target_sources(backend
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
        include/audioadapter.h
)

target_sources(backend PRIVATE
    src/audioadapter.cpp
)

target_include_directories(backend
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_link_libraries(backend PRIVATE
  engine
  rtaudio
)

set_target_properties(backend PROPERTIES LINKER_LANGUAGE CXX)
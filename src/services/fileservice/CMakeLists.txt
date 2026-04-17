add_library(fileservice STATIC)

target_sources(fileservice
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_SOURCE_DIR}/include
    FILES
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/fileservice.h
)

target_sources(fileservice PRIVATE
  src/fileservice.cpp
)

target_include_directories(fileservice
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(fileservice
  PUBLIC
    core
)

set_target_properties(fileservice PROPERTIES LINKER_LANGUAGE CXX)
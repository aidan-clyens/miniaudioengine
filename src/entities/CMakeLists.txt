add_library(entities STATIC)

target_sources(entities
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/device.h
      include/file.h
)

target_sources(entities PRIVATE
  src/device.cpp
  src/file.cpp
)

target_include_directories(entities
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_link_libraries(entities
  PRIVATE
    framework
    adapters
)

set_target_properties(entities PROPERTIES LINKER_LANGUAGE CXX)
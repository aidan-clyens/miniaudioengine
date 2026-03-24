add_library(shared STATIC)

target_sources(shared
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_SOURCE_DIR}/include
    FILES
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/devicehandle.h
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/filehandle.h
)

target_sources(shared PRIVATE
  src/devicehandle.cpp
  src/filehandle.cpp
)

target_include_directories(shared
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(shared
  PUBLIC
    sndfile
    framework
)

set_target_properties(shared PROPERTIES LINKER_LANGUAGE CXX)
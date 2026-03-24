add_library(framework STATIC)

target_sources(framework
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/messagequeue.h
      include/lockfree_ringbuffer.h
      include/doublebuffer.h
      include/logger.h
      include/input.h
      include/controller.h
      include/dataplane.h
      include/processor.h
      include/device.h
      include/miditypes.h
)

target_sources(framework
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_SOURCE_DIR}/include
    FILES
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/devicehandle.h
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/filehandle.h
)

target_sources(framework PRIVATE
  src/logger.cpp
  src/controller.cpp
  src/devicehandle.cpp
  src/filehandle.cpp
)

target_include_directories(framework
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_link_libraries(framework
  PUBLIC
    sndfile
)

set_target_properties(framework PROPERTIES LINKER_LANGUAGE CXX)
add_library(core STATIC)

target_sources(core
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/messagequeue.h
      include/lockfree_ringbuffer.h
      include/doublebuffer.h
      include/logger.h
      include/miditypes.h
      include/interfaces/controller.h
      include/interfaces/dataplane.h
      include/interfaces/processor.h
      include/interfaces/device.h
)

target_sources(core
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_SOURCE_DIR}/include
    FILES
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/devicehandle.h
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/filehandle.h
)

target_sources(core PRIVATE
  src/logger.cpp
  src/controller.cpp
  src/devicehandle.cpp
  src/filehandle.cpp
)

target_include_directories(core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_link_libraries(core
  PUBLIC
    sndfile
)

set_target_properties(core PROPERTIES LINKER_LANGUAGE CXX)
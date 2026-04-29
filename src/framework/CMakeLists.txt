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
      include/miditypes.h
      include/device.h
      include/file.h
      include/interfaces/controller.h
      include/interfaces/dataplane.h
      include/interfaces/processor.h
      include/interfaces/device.h
)

target_sources(framework PRIVATE
  src/logger.cpp
  src/controller.cpp
  src/device.cpp
  src/file.cpp
)

target_include_directories(framework
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_link_libraries(framework
  PRIVATE
    adapters
)

set_target_properties(framework PROPERTIES LINKER_LANGUAGE CXX)
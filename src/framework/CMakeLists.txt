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
      include/observer.h
      include/subject.h
      include/engine.h
      include/logger.h
      include/input.h
      include/controller.h
      include/manager.h
)

target_sources(framework PRIVATE 
  src/logger.cpp
)

target_include_directories(framework
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set_target_properties(framework PROPERTIES LINKER_LANGUAGE CXX)
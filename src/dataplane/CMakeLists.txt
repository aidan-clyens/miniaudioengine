add_library(dataplane STATIC)

target_sources(dataplane
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
        include/audiograph.h
        include/inputnode.h
        include/outputnode.h
        include/processornode.h
        include/mixernode.h
)

target_sources(dataplane PRIVATE
    src/audiograph.cpp
    src/inputnode.cpp
    src/outputnode.cpp
    src/processornode.cpp
    src/mixernode.cpp
)

target_include_directories(dataplane
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

set_target_properties(dataplane PROPERTIES LINKER_LANGUAGE CXX)
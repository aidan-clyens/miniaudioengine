add_library(adapters STATIC)

target_sources(adapters
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
        include/audioadapter.h
        include/midiadapter.h
        include/fileadapter.h
)

target_sources(adapters PRIVATE
    src/audioadapter.cpp
    src/midiadapter.cpp
    src/fileadapter.cpp
)

target_include_directories(adapters
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_link_libraries(adapters PRIVATE
  framework
  entities
)

target_link_libraries(adapters PUBLIC
  rtaudio
  rtmidi
  sndfile
)

set_target_properties(adapters PROPERTIES LINKER_LANGUAGE CXX)
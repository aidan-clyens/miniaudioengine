add_library(control-midi STATIC)

target_sources(control-midi
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/midicontroller.h
)

target_sources(control-midi
  PRIVATE
    src/midicontroller.cpp
)

target_include_directories(control-midi
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(control-midi
  PUBLIC
    framework
    rtmidi
    data-midi
)

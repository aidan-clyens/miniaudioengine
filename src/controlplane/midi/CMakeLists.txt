add_library(controlplane-midi STATIC)

target_sources(controlplane-midi
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/midiportcontroller.h
      include/miditypes.h
)

target_sources(controlplane-midi
  PRIVATE
    src/midiportcontroller.cpp
)

target_include_directories(controlplane-midi
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(controlplane-midi
  PUBLIC
    framework
    rtmidi
    dataplane-midi
    trackmanager
)

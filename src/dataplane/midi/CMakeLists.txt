add_library(dataplane-midi)

target_sources(dataplane-midi
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/midicontroltypes.h
      include/midicallbackhandler.h
      include/mididataplane.h
)

target_sources(dataplane-midi
  PRIVATE
  src/midicallbackhandler.cpp
  src/mididataplane.cpp
)

target_include_directories(dataplane-midi
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(dataplane-midi PUBLIC
  rtmidi
  framework
  controlplane-midi
)

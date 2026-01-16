add_library(data-midi)

target_sources(data-midi
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/midicontroltypes.h
      include/midicallbackhandler.h
      include/mididataplane.h
)

target_sources(data-midi
  PRIVATE
  src/midicallbackhandler.cpp
  src/mididataplane.cpp
)

target_include_directories(data-midi
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(data-midi PUBLIC
  rtmidi
  framework
  control-midi
)

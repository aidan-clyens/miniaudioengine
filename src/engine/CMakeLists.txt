add_library(engine STATIC)

target_sources(engine
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/engine.h
      include/audiodataplane.h
      include/audiocontroller.h
      include/midicontroltypes.h
      include/midicallbackhandler.h
      include/mididataplane.h
      include/midicontroller.h
)

target_sources(engine PRIVATE
  src/engine.cpp
  src/audiodataplane.cpp
  src/audiocontroller.cpp
  src/midicallbackhandler.cpp
  src/mididataplane.cpp
  src/midicontroller.cpp
)

target_include_directories(engine
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_link_libraries(engine PUBLIC
  core
  backend
  processor-audio
  rtmidi
)

set_target_properties(engine PROPERTIES LINKER_LANGUAGE CXX)
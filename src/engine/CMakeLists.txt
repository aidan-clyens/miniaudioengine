add_library(engine STATIC)

target_sources(engine
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/audiodataplane.h
      include/audiocontroller.h
      include/midicontroltypes.h
      include/mididataplane.h
      include/midicontroller.h
)

target_sources(engine PRIVATE
  src/audiodataplane.cpp
  src/audiocontroller.cpp
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
  adapters
  processor-audio
)

set_target_properties(engine PROPERTIES LINKER_LANGUAGE CXX)
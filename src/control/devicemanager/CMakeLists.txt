add_library(devicemanager STATIC)

target_sources(devicemanager
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/devicemanager.h
      include/audiodevice.h
      include/mididevice.h
)

target_sources(devicemanager PRIVATE
  src/devicemanager.cpp
)

target_include_directories(devicemanager
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(devicemanager
  PUBLIC
    data-audio
    data-midi
    control-audio
    control-midi
)

set_target_properties(devicemanager PROPERTIES LINKER_LANGUAGE CXX)
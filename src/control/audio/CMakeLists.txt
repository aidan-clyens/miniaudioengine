add_library(control-audio STATIC)

target_sources(control-audio
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/audiocontroller.h
      include/audiostreamcontroller.h
)

target_sources(control-audio PRIVATE
  src/audiocontroller.cpp
  src/audiostreamcontroller.cpp
)

target_include_directories(control-audio
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(control-audio
  PUBLIC
    rtaudio
    framework
    data-audio
    devicemanager
)

set_target_properties(control-audio PROPERTIES LINKER_LANGUAGE CXX)
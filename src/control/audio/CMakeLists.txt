add_library(control-audio STATIC)

target_sources(control-audio
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/audiocontroller_interface.h
      include/audiocontroller.h
)

target_sources(control-audio PRIVATE
  src/audiocontroller_interface.cpp
  src/audiocontroller.cpp
)

target_include_directories(control-audio
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/public/io/devicemanager/include>
)

target_link_libraries(control-audio
  PUBLIC
    rtaudio
    framework
    data-audio
    devicemanager
)

set_target_properties(control-audio PROPERTIES LINKER_LANGUAGE CXX)
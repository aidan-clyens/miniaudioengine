add_library(controlplane-audio STATIC)

target_sources(controlplane-audio
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/audiostreamcontroller.h
)

target_sources(controlplane-audio PRIVATE
  src/audiostreamcontroller.cpp
)

target_include_directories(controlplane-audio
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(controlplane-audio
  PUBLIC
    rtaudio
    devicemanager
)

set_target_properties(controlplane-audio PROPERTIES LINKER_LANGUAGE CXX)
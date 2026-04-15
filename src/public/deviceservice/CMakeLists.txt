add_library(deviceservice STATIC)

target_sources(deviceservice
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_SOURCE_DIR}/include
    FILES
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/deviceservice.h
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/audiodevice.h
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/mididevice.h
)

target_sources(deviceservice PRIVATE
  src/deviceservice.cpp
)

target_include_directories(deviceservice
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(deviceservice
  PUBLIC
    core
    data-audio
    data-midi
    control-audio
    control-midi
)

set_target_properties(deviceservice PROPERTIES LINKER_LANGUAGE CXX)
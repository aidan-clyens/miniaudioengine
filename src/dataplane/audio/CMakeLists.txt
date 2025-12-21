add_library(dataplane-audio STATIC)

target_sources(dataplane-audio
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS 
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/audioengine.h
      include/audiointerface.h
      include/audioprocessor.h
      include/oscillator.h
)

target_sources(dataplane-audio PRIVATE src/audiointerface.cpp src/audioengine.cpp)

target_include_directories(dataplane-audio
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(dataplane-audio PUBLIC
  rtaudio
  framework
  devicemanager
  trackmanager
)

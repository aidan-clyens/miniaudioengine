add_library(data-audio STATIC)

target_sources(data-audio
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS 
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/audiodataplane.h
      include/audiocallbackhandler.h
)

target_sources(data-audio PRIVATE
  src/audiocallbackhandler.cpp
  src/audiodataplane.cpp
)

target_include_directories(data-audio
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(data-audio PUBLIC
  rtaudio
  framework
  devicemanager
  trackmanager
  processor-audio
)

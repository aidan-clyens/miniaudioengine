add_library(trackmanager STATIC)

target_sources(trackmanager
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_SOURCE_DIR}/include
    FILES
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/track.h
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/trackmanager.h
)

target_sources(trackmanager
  PRIVATE
  src/trackmanager.cpp
  src/track.cpp
)

target_include_directories(trackmanager
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_link_libraries(trackmanager PUBLIC
  framework
  control-audio
  control-midi
  data-midi
  data-audio
  processor-audio
  filemanager
  devicemanager
)

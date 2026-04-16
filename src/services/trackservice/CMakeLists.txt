add_library(trackservice STATIC)

target_sources(trackservice
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_SOURCE_DIR}/include
    FILES
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/track.h
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/trackservice.h
)

target_sources(trackservice
  PRIVATE
  src/trackservice.cpp
  src/track.cpp
)

target_include_directories(trackservice
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_link_libraries(trackservice PUBLIC
  core
  engine
  processor-audio
  fileservice
  deviceservice
)

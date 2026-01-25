add_library(processor-sampleplayer STATIC)

target_sources(processor-sampleplayer
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/sampleplayer.h
)

target_sources(processor-sampleplayer PRIVATE
  src/sampleplayer.cpp
)

target_include_directories(processor-sampleplayer
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(processor-sampleplayer
  PUBLIC
    framework
    processor-audio
)

set_target_properties(processor-sampleplayer PROPERTIES LINKER_LANGUAGE CXX)
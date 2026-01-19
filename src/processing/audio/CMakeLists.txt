add_library(processor-audio STATIC)

target_sources(processor-audio
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/audioprocessor.h
)

# target_sources(processor-audio PRIVATE

# )

target_include_directories(processor-audio
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(processor-audio
  PUBLIC
    framework
)

set_target_properties(processor-audio PROPERTIES LINKER_LANGUAGE CXX)
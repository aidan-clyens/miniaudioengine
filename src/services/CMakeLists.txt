add_library(services STATIC)

target_sources(services
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/track.h
      include/trackservice.h
      include/deviceservice.h
      include/fileservice.h
)

target_sources(services PRIVATE
    src/deviceservice.cpp
    src/fileservice.cpp
    src/trackservice.cpp
    src/track.cpp
)

target_include_directories(services
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_link_libraries(services PUBLIC
  framework
  engine
)

set_target_properties(services PROPERTIES LINKER_LANGUAGE CXX)
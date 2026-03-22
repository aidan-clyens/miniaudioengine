add_library(filemanager STATIC)

target_sources(filemanager
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_SOURCE_DIR}/include
    FILES
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/filemanager.h
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/filehandle.h
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/wavfile.h
      ${CMAKE_SOURCE_DIR}/include/miniaudioengine/midifile.h
)

target_sources(filemanager PRIVATE
  src/filemanager.cpp
  src/filehandle.cpp
)

target_include_directories(filemanager
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(filemanager
  PUBLIC
    sndfile
    framework
)

set_target_properties(filemanager PROPERTIES LINKER_LANGUAGE CXX)
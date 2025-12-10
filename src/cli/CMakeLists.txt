add_library(cli STATIC)

target_sources(cli
  PUBLIC
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/cli.h
)

target_sources(cli PRIVATE
  src/cli.cpp
)

target_include_directories(cli
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

find_package(Threads REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(replxx CONFIG REQUIRED)

target_link_libraries(cli PUBLIC
  coreengine
  Threads::Threads
  CLI11::CLI11
  replxx::replxx
)

set_target_properties(cli PROPERTIES LINKER_LANGUAGE CXX)
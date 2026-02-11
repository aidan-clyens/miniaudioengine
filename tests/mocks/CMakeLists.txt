add_library(test-mocks INTERFACE)

target_sources(test-mocks
  INTERFACE
  FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
      include/dataplane_mock.h
      include/controller_mock.h
      include/device_mock.h
      include/audiocontroller_mock.h
)

target_include_directories(test-mocks
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(test-mocks
  INTERFACE
    framework
    control-audio
    data-audio
    trackmanager
)
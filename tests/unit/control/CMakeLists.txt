add_library(miniaudioengine-unit-test-control STATIC
  test_audiostreamcontroller_unit.cpp
  test_midiportcontroller_unit.cpp
  test_filemanager_unit.cpp
  test_trackmanager_unit.cpp
  test_track_unit.cpp
  test_devicemanager_unit.cpp
)

target_link_libraries(miniaudioengine-unit-test-control PRIVATE
  GTest::gtest
  GTest::gtest_main
  test-mocks
  control-audio
  control-midi
)

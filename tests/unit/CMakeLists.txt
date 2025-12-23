enable_testing()

add_executable(minimal-audio-engine-unit-tests
  test_audiostreamcontroller_unit.cpp
  test_filemanager_unit.cpp
  test_trackmanager_unit.cpp
  test_track_unit.cpp
  test_devicemanager_unit.cpp
)

target_link_libraries(minimal-audio-engine-unit-tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  controlplane-audio
  dataplane-audio
  trackmanager
  filemanager
  devicemanager
)

add_test(NAME minimal-audio-engine-unit-tests COMMAND minimal-audio-engine-unit-tests)
enable_testing()

add_executable(miniaudioengine-unit-tests
  test_audiostreamcontroller_unit.cpp
  test_midiportcontroller_unit.cpp
  test_filemanager_unit.cpp
  test_trackmanager_unit.cpp
  test_track_unit.cpp
  test_trackaudiodataplane_unit.cpp
  test_trackmididataplane_unit.cpp
  test_devicemanager_unit.cpp
  test_messagequeue_unit.cpp
  test_lockfree_ringbuffer_unit.cpp
  test_doublebuffer_unit.cpp
)

target_link_libraries(miniaudioengine-unit-tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  control-audio
  control-midi
  data-audio
  data-midi
  trackmanager
  filemanager
  devicemanager
  test-mocks
)

add_test(NAME miniaudioengine-unit-tests COMMAND miniaudioengine-unit-tests)
set_tests_properties(miniaudioengine-unit-tests PROPERTIES TIMEOUT 60)
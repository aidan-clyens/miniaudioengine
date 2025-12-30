enable_testing()

add_executable(minimal-audio-engine-unit-tests
  # test_audiostreamcontroller_unit.cpp
  # test_midiportcontroller_unit.cpp
  # test_filemanager_unit.cpp
  # test_trackmanager_unit.cpp
  # test_track_unit.cpp
  # test_trackaudiodataplane_unit.cpp
  test_trackmididataplane_unit.cpp
  # test_devicemanager_unit.cpp
  # test_messagequeue_unit.cpp
  # test_subject_observer_unit.cpp
  # test_lockfree_ringbuffer_unit.cpp
  # test_doublebuffer_unit.cpp
)

target_link_libraries(minimal-audio-engine-unit-tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  controlplane-audio
  controlplane-midi
  dataplane-audio
  dataplane-midi
  trackmanager
  filemanager
  devicemanager
)

add_test(NAME minimal-audio-engine-unit-tests COMMAND minimal-audio-engine-unit-tests)
set_tests_properties(minimal-audio-engine-unit-tests PROPERTIES TIMEOUT 60)
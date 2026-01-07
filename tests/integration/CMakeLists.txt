enable_testing()

add_executable(minimal-audio-engine-integration-tests
  test_audio_input_integration.cpp
  test_midi_input_integration.cpp
)

target_link_libraries(minimal-audio-engine-integration-tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  framework
  controlplane-audio
  dataplane-audio
  dataplane-midi
  trackmanager
  filemanager
  devicemanager
)

add_test(NAME minimal-audio-engine-integration-tests COMMAND minimal-audio-engine-integration-tests)
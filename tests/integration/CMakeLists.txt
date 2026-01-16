enable_testing()

add_executable(miniaudioengine-integration-tests
  test_audio_input_integration.cpp
  test_midi_input_integration.cpp
)

target_link_libraries(miniaudioengine-integration-tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  framework
  control-audio
  data-audio
  data-midi
  trackmanager
  filemanager
  devicemanager
)

add_test(NAME miniaudioengine-integration-tests COMMAND miniaudioengine-integration-tests)
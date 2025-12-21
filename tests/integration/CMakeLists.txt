enable_testing()

add_executable(EmbeddedAudioEngineTests
  test_audio_input_integration.cpp
  test_midi_input_integration.cpp
)

target_link_libraries(EmbeddedAudioEngineTests PRIVATE
  GTest::gtest
  GTest::gtest_main
  framework
  dataplane-audio
  dataplane-midi
  trackmanager
  filemanager
  devicemanager
)

add_test(NAME EmbeddedAudioEngineUnitTests COMMAND EmbeddedAudioEngineUnitTests)
add_test(NAME EmbeddedAudioEngineTests COMMAND EmbeddedAudioEngineTests)
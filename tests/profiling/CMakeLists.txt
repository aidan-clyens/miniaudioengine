enable_testing()

add_executable(miniaudioengine-profiling-tests
  # test_lockfree_ringbuffer_profiling.cpp
  test_track_wav_file_profiling.cpp
)

target_link_libraries(miniaudioengine-profiling-tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  framework
  trackmanager
  devicemanager
  data-audio
  control-audio
)

add_test(NAME miniaudioengine-profiling-tests COMMAND miniaudioengine-profiling-tests)
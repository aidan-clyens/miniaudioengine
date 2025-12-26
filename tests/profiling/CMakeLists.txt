enable_testing()

add_executable(minimal-audio-engine-profiling-tests
  test_lockfree_ringbuffer_profiling.cpp
)

target_link_libraries(minimal-audio-engine-profiling-tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  framework
)

add_test(NAME minimal-audio-engine-profiling-tests COMMAND minimal-audio-engine-profiling-tests)
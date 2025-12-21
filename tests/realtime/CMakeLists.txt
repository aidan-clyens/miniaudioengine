enable_testing()

add_executable(minimal-audio-engine-realtime-tests
  test_lockfree_ringbuffer.cpp
)

target_link_libraries(minimal-audio-engine-realtime-tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  framework
)

add_test(NAME minimal-audio-engine-realtime-tests COMMAND minimal-audio-engine-realtime-tests)
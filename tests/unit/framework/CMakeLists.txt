add_library(miniaudioengine-unit-test-framework STATIC
  test_messagequeue_unit.cpp
  test_lockfree_ringbuffer_unit.cpp
  test_doublebuffer_unit.cpp
)

target_link_libraries(miniaudioengine-unit-test-framework PRIVATE
  GTest::gtest
  GTest::gtest_main
  framework
  test-mocks
)

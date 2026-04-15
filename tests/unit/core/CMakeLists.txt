add_library(miniaudioengine-unit-test-core STATIC
  test_messagequeue_unit.cpp
  test_lockfree_ringbuffer_unit.cpp
  test_doublebuffer_unit.cpp
  test_controller_unit.cpp
  test_dataplane_unit.cpp
)

target_link_libraries(miniaudioengine-unit-test-core PRIVATE
  GTest::gtest
  GTest::gtest_main
  core
  test-mocks
)

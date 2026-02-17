add_library(miniaudioengine-unit-test-data STATIC
  test_audiodataplane_unit.cpp
  test_mididataplane_unit.cpp
)

target_link_libraries(miniaudioengine-unit-test-data PRIVATE
  GTest::gtest
  GTest::gtest_main
  test-mocks
  data-audio
  data-midi
)

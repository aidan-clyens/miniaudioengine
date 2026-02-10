add_library(miniaudioengine-unit-test-data STATIC
  test_trackaudiodataplane_unit.cpp
  test_trackmididataplane_unit.cpp
)

target_link_libraries(miniaudioengine-unit-test-data PRIVATE
  GTest::gtest
  GTest::gtest_main
  test-mocks
  data-audio
  data-midi
)

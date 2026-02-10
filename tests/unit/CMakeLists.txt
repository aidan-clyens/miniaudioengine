add_subdirectory(framework)
add_subdirectory(control)
add_subdirectory(data)

add_executable(miniaudioengine-unit-tests
  test_main.cpp
)

target_link_libraries(miniaudioengine-unit-tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  miniaudioengine-unit-test-framework
  miniaudioengine-unit-test-control
  miniaudioengine-unit-test-data
  control-audio
  control-midi
  data-audio
  data-midi
  trackmanager
  filemanager
  devicemanager
  test-mocks
)

target_link_options(miniaudioengine-unit-tests PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/WHOLEARCHIVE:miniaudioengine-unit-test-framework>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wl,--whole-archive>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:miniaudioengine-unit-test-framework>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wl,--no-whole-archive>
  $<$<CXX_COMPILER_ID:MSVC>:/WHOLEARCHIVE:miniaudioengine-unit-test-control>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wl,--whole-archive>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:miniaudioengine-unit-test-control>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wl,--no-whole-archive>
  $<$<CXX_COMPILER_ID:MSVC>:/WHOLEARCHIVE:miniaudioengine-unit-test-data>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wl,--whole-archive>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:miniaudioengine-unit-test-data>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wl,--no-whole-archive>
)

include(GoogleTest)
gtest_discover_tests(miniaudioengine-unit-tests
  DISCOVERY_TIMEOUT 60
  PROPERTIES TIMEOUT 60
  TEST_LIST miniaudioengine_unit_test_list
)

foreach(test_name IN LISTS miniaudioengine_unit_test_list)
  if(test_name MATCHES "^([^.]+)\\.")
    set(suite_name "${CMAKE_MATCH_1}")
    set_tests_properties(${test_name} PROPERTIES LABELS "${suite_name}")
  endif()
endforeach()
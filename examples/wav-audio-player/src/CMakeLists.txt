add_executable(
  ${PROJECT_NAME}
  main.cpp
)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE
  core
  trackservice
  deviceservice
  fileservice
  cli
)
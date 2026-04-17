add_executable(
  ${PROJECT_NAME}
  main.cpp
)

target_link_libraries(
  ${PROJECT_NAME} 
  core
  trackservice
  deviceservice
  cli
)

target_compile_definitions(
  ${PROJECT_NAME} PUBLIC
  -DPROGRAM_NAME="${PROJECT_NAME}"
)
add_executable(
  ${PROJECT_NAME}
  main.cpp
)

target_link_libraries(
  ${PROJECT_NAME} 
  framework
  trackmanager
  devicemanager
  cli
)

target_compile_definitions(
  ${PROJECT_NAME} PUBLIC
  -DPROGRAM_NAME="${PROJECT_NAME}"
)
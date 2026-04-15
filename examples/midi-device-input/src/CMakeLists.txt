add_executable(
  ${PROJECT_NAME}
  main.cpp
)

target_link_libraries(
  ${PROJECT_NAME} 
  core
  trackmanager
  deviceservice
  cli
  rtaudio
  rtmidi
  sndfile
)

target_compile_definitions(
  ${PROJECT_NAME} PUBLIC
  -DPROGRAM_NAME="${PROJECT_NAME}"
)
add_executable(
  ${PROJECT_NAME}
  main.cpp
)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE
  core
  trackmanager
  devicemanager
  filemanager
  cli
  rtaudio
  rtmidi
  sndfile
)
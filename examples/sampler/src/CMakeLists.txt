add_executable(
  ${PROJECT_NAME}
  main.cpp
)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE
    core
    trackmanager
    deviceservice
    filemanager
    control-midi
    data-midi
    processor-audio
    processor-sampleplayer
    cli
    rtaudio
    rtmidi
    sndfile
)
add_executable(
  ${PROJECT_NAME}
  main.cpp
)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE
    framework
    trackmanager
    devicemanager
    filemanager
    data-midi
    processor-audio
    processor-sampleplayer
    cli
)
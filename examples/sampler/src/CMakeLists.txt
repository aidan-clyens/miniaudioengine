add_executable(
  ${PROJECT_NAME}
  main.cpp
)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE
    framework
    services
    engine
    processor-audio
    processor-sampleplayer
    cli
)
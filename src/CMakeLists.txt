add_subdirectory(cli)
add_subdirectory(coreengine)

add_executable(EmbeddedAudioEngine
  main.cpp
)

target_link_libraries(EmbeddedAudioEngine PRIVATE
  cli
)

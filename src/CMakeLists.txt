add_subdirectory(framework)
add_subdirectory(audioengine)
add_subdirectory(midiengine)
add_subdirectory(trackmanager)
add_subdirectory(filemanager)
add_subdirectory(devicemanager)
add_subdirectory(coreengine)

add_executable(EmbeddedAudioEngine
  main.cpp
)

target_link_libraries(EmbeddedAudioEngine PRIVATE
  audioengine
  midiengine
  trackmanager
  filemanager
  devicemanager
  coreengine
)

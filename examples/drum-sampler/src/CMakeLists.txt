add_executable(
  ${PROJECT_NAME}
  main.cpp
  Clock.hpp
  sequencer.hpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE audiosession)
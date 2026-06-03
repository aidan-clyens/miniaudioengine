#include <iostream>
#include <csignal>
#include <string>
#include <optional>
#include <vector>
#include <thread>
#include <chrono>

#include <CLI11.hpp>

#include "audiosession.h"
#include "track.h"
#include "file.h"
#include "logger.h"

// TODO - Get version from CMake
#ifndef VERSION_NUMBER
#define VERSION_NUMBER "1.0.0"
#endif

using namespace miniaudioengine;

constexpr const char* APP_NAME = "device-info";
constexpr const char *APP_DESCRIPTION = "Info about the device audio/MIDI API.";
constexpr const char* APP_LOG_FILEPATH = "device-info.log";

constexpr const char* ARG_VERBOSE = "--verbose";

/** @brief List available audio devices.
 *  @param session Reference to the AudioSession
 */
void list_audio_devices(const AudioSession &session)
{
  DeviceList audio_devices = session.get_audio_devices();
  std::cout << "Available Audio Devices:\n";
  for (const auto& device : audio_devices) {
    std::cout << device->to_string() << "\n";
  }
}

/** @brief Parses command line arguments and configures the audio session.
 *  @param argc Argument count from main()
 *  @param argv Argument vector from main()
 *  @param session Reference to the AudioSession to configure.
 *  @return 0 on success, non-zero on failure.
 */
int parse_cli_arguments(int argc, char *argv[], AudioSession &session)
{
  // Parse command line arguments
  CLI::App app{std::string(APP_NAME) + " - " + APP_DESCRIPTION};
  argv = app.ensure_utf8(argv);

  // --verbose flag
  app.add_flag_callback(ARG_VERBOSE, []()
                        { framework::Logger::instance().enable_console_output(true); }, "Enable verbose logging");

  CLI11_PARSE(app, argc, argv);
  return 0;
}

/** @brief Main entry point for the WAV audio player application.
 */
int main(int argc, char *argv[])
{
  // Setup logging
  framework::Logger::instance().enable_console_output(false);
  framework::Logger::instance().set_log_file(APP_LOG_FILEPATH);
  framework::set_thread_name("Main");

  std::cout << APP_NAME << " - " << APP_DESCRIPTION << std::endl;
  std::cout << "Saving logs to " << APP_LOG_FILEPATH << std::endl;

  // Create Audio Session
  AudioSession session;

  if (parse_cli_arguments(argc, argv, session) != 0) {
    return -1;
  }

  list_audio_devices(session);

  return 0;
}
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

constexpr const char* APP_NAME = "midi-input";
constexpr const char *APP_DESCRIPTION = "Monitor input from a MIDI device.";
constexpr const char* APP_LOG_FILEPATH = "midi-input.log";

constexpr const char *ARG_LIST_MIDI_DEVICES = "--list-midi-devices";
constexpr const char *ARG_MIDI_INPUT_ID = "--midi-input";
constexpr const char* ARG_VERBOSE = "--verbose";

/** @brief List available MIDI devices.
 *  @param session Reference to the AudioSession
 */
void list_midi_devices(const AudioSession &session)
{
  DeviceList midi_devices = session.get_midi_devices();
  std::cout << "Available MIDI Devices:\n";
  for (const auto &device : midi_devices)
  {
    std::cout << device->to_string() << "\n";
  }
}

/** @brief Set the MIDI input device.
 *  @param session Reference to the AudioSession
 *  @param device_id ID of the MIDI input device
 *  @return true if the device was set
 *  @return false if the device could not be set
 */
bool set_midi_input_device(AudioSession &session, unsigned int device_id)
{
  try
  {
    DevicePtr midi_device = session.get_midi_device(device_id);
    TrackPtr track = session.add_track();
    track->add_midi_input(midi_device);
    // track->add_audio_output(session.get_default_audio_output_device());
  }
  catch (std::exception &e)
  {
    std::cerr << "Error: No MIDI device found with ID " << device_id << ".\n";
    return false;
  }

  return true;
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

  // --list-midi-devices flag
  app.add_flag_callback(
      ARG_LIST_MIDI_DEVICES,
      [&session]()
      {
        list_midi_devices(session);
        std::exit(0);
      },
      "List available audio devices");

  // --midi-input-id option
  app.add_option_function<unsigned int>(
      ARG_MIDI_INPUT_ID,
      [&session](const unsigned int &device_id)
      {
        if (!set_midi_input_device(session, device_id))
        {
          std::exit(1);
        }
      },
      "Specify MIDI input device by ID");

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

  if (parse_cli_arguments(argc, argv, session) != 0)
  {
    return -1;
  }

  if (!session.play())
  {
    std::cerr << "midi-input: Failed to play" << std::endl;
    return 1;
  }

  while (session.get_state() == eAudioSessionState::Playing)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}
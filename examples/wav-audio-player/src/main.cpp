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

constexpr const char* APP_NAME = "wav-audio-player";
constexpr const char* APP_DESCRIPTION = "A simple audio player using the miniaudioengine library that can play .wav files";
constexpr const char* APP_LOG_FILEPATH = "wav-audio-player.log";

constexpr const char* ARG_LIST_AUDIO_DEVICES = "--list-audio-devices";
constexpr const char* ARG_SET_INPUT_FILE = "--input-file";
constexpr const char* ARG_SET_AUDIO_OUTPUT = "--set-audio-output";
constexpr const char* ARG_VERBOSE = "--verbose";

static bool running = true;

static DevicePtr audio_output_device = nullptr;
static FilePtr audio_file = nullptr;

/** @brief Read an audio file and set it as the input on the track.
 *  @param session Reference to the AudioSession
 *  @param file_path Path to the input file
 *  @return true if the file was set
 *  @return false if the file could not be set
 */
bool set_input_file(AudioSession &session, const std::string &file_path)
{
  FilePtr file = session.get_audio_file(file_path);
  if (file == nullptr) {
    std::cerr << "Error: Unable to open audio file: " << file_path << "\n";
    return false;
  }
  audio_file = file;
  return true;
}

/** @brief Set the audio output device.
 *  @param session Reference to the AudioSession
 *  @param device_id ID of the audio output device
 *  @return true if the device was set
 *  @return false if the device could not be set
 */
bool set_output_device(AudioSession &session, unsigned int device_id)
{
  DevicePtr audio_device = session.get_audio_device(device_id);
  if (audio_device->get_id() != device_id) {
    std::cerr << "Error: No audio device found with ID " << device_id << ".\n";
    return false;
  }
  audio_output_device = audio_device;
  return true;
}

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

  // --list-audio-devices flag
  app.add_flag_callback(
    ARG_LIST_AUDIO_DEVICES,
    [&session]()
    {
      list_audio_devices(session);
      std::exit(0);
    },
    "List available audio devices"
  );

  // --input-file option
  app.add_option_function<std::string>(
    ARG_SET_INPUT_FILE,
    [&session](const std::string &arg)
    {
      if (!set_input_file(session, arg))
      {
        std::exit(1);
      }
    },
    "Specify input WAV file"
  );

  // --set-audio-output option
  app.add_option_function<unsigned int>(
    ARG_SET_AUDIO_OUTPUT,
    [&session](const unsigned int &device_id)
    {
      if (!set_output_device(session, device_id))
      {
        std::exit(1);
      }
    },
    "Specify audio output device by ID"
  );

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

  // Create Audio Session
  AudioSession session;

  if (parse_cli_arguments(argc, argv, session) != 0) {
    return -1;
  }

  LOG_INFO("Initializing ", APP_NAME, "...");

  // Handle SIGINT (Ctrl+C) for graceful shutdown
  std::signal(SIGINT, [](int) {
    running = false;
  });

  // Add one track
  TrackPtr track = session.add_track();
  if (track == nullptr) {
    LOG_ERROR("Failed to create track.");
    return -1;
  }

  // Read input file and set as audio input on track
  if (audio_file) {
    track->add_audio_input(audio_file);
    std::cout << "Selected Audio Input: " << audio_file->to_string() << "\n";
  } else {
    LOG_ERROR("No valid audio file selected.");
    return -1;
  }

  // Set audio output device
  if (audio_output_device) {
    track->add_audio_output(audio_output_device);
    std::cout << "Selected Audio Output: " << audio_output_device->to_string() << "\n";
  } else {
    LOG_ERROR("No audio output device found.");
    return -1;
  }

  if (!track->has_audio_input()) {
    LOG_ERROR("Failed to set audio input on track.");
    return -1;
  }

  if (!track->has_audio_output()) {
    LOG_ERROR("Failed to set audio output on track.");
    return -1;
  }

  // Start playback
  session.play();
  if (session.get_state() != eAudioSessionState::Playing)
  {
    std::cerr << "Could not play " << audio_file->get_filename() << "!" << std::endl;
    return -1;
  }

  std::cout << "Playing... Press Ctrl+C to stop.\n";

  // Main application loop
  while (session.get_state() == eAudioSessionState::Playing)
  {
    if (!running)
    {
      std::cout << "Stopping..." << std::endl;
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  // Stop playback
  session.stop();
  if (session.get_state() != eAudioSessionState::Stopped)
  {
    LOG_ERROR("Failed to stop playback.");
    return -1;
  }

  return 0;
}
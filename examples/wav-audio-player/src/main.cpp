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
using namespace miniaudioengine::framework;

static bool running = true;
static std::optional<std::string> input_file_path = std::nullopt;
static std::optional<unsigned int> audio_output_device_id = std::nullopt;

/** @brief Parses command line arguments and configures the audio session.
 *  @param argc Argument count from main()
 *  @param argv Argument vector from main()
 *  @param session Reference to the AudioSession to configure.
 *  @return 0 on success, non-zero on failure.
 */
int parse_cli_arguments(int argc, char *argv[], AudioSession &session)
{
  // Parse command line arguments
  CLI::App app{"WAV Audio Player - A simple audio player using the miniaudioengine library that can play .wav files"};
  argv = app.ensure_utf8(argv);

  // --input-file option
  app.add_option_function<std::string>("--input-file", [](const std::string &arg)
                                       { input_file_path = arg; }, "Specify input WAV file");

  // --list-audio-devices flag
  app.add_flag_callback("--list-audio-devices", [&session]()
                        {
    DeviceList audio_devices = session.get_audio_devices();
    std::cout << "Available Audio Devices:\n";
    for (const auto& device : audio_devices) {
      std::cout << device->to_string() << "\n";
    }
    std::exit(0); }, "List available audio devices");

  // --set-audio-output option
  app.add_option_function<unsigned int>("--set-audio-output", [&session](const unsigned int &device_id)
                                        {
    DevicePtr audio_device = session.get_audio_device(device_id);
    if (audio_device->get_id() != device_id) {
      std::cerr << "Error: No audio device found with ID " << device_id << ".\n";
      std::exit(1);
    }
    std::cout << "Selected Audio Output Device: " << audio_device->to_string() << "\n";
    audio_output_device_id = device_id; }, "Specify audio output device by ID");

  // --verbose flag
  app.add_flag_callback("--verbose", []()
                        { Logger::instance().enable_console_output(true); }, "Enable verbose logging");

  CLI11_PARSE(app, argc, argv);
  return 0;
}

/** @brief Main entry point for the WAV audio player application.
 */
int main(int argc, char *argv[])
{
  // Setup logging
  Logger::instance().enable_console_output(false);
  Logger::instance().set_log_file("WavAudioPlayer.log");
  set_thread_name("Main");

  // Create Audio Session
  AudioSession session;

  if (parse_cli_arguments(argc, argv, session) != 0) {
    return -1;
  }

  LOG_INFO("Initializing wav-audio-player...");

  FilePtr audio_file = nullptr;
  DevicePtr output_device = nullptr;

  // Handle SIGINT (Ctrl+C) for graceful shutdown
  std::signal(SIGINT, [](int) {
    LOG_INFO("SIGINT received, shutting down...");
    running = false;
  });

  // Add one track
  TrackPtr track = session.add_track();
  if (track == nullptr) {
    LOG_ERROR("Failed to create track.");
    return -1;
  }

  // Read input file and set as audio input on track
  audio_file = (input_file_path.has_value()) ? session.get_audio_file(input_file_path.value()) : nullptr;
  if (audio_file) {
    std::cout << "Selected audio file: " << audio_file->to_string() << "\n";
    track->add_audio_input(audio_file);
    LOG_INFO("Set audio file as input: ", audio_file->to_string());
  } else {
    LOG_ERROR("No valid audio file selected.");
    return -1;
  }

  // Set audio output device
  output_device = (audio_output_device_id.has_value()) ?
    session.get_audio_device(audio_output_device_id.value()) : session.get_default_audio_output_device();

  if (output_device) {
    std::cout << "Using Audio Output Device: " << output_device->to_string() << "\n";
    track->add_audio_output(output_device);
    LOG_INFO("Set audio output device: ", output_device->to_string());
  } else {
    LOG_ERROR("No audio output device found.");
    return -1;
  }

  // Open WAV file and add as audio input
  FilePtr wav_file = session.get_audio_file(input_file_path.value());
  if (wav_file == nullptr) {
    LOG_ERROR("Failed to read WAV file: ", input_file_path.value());
    return -1;
  }
  
  std::cout << "Playing WAV file: " << wav_file->to_string() << "\n";
  track->add_audio_input(wav_file);
  LOG_INFO("Set WAV file as audio input: ", wav_file->to_string());
  
  if (!track->has_audio_input()) {
    LOG_ERROR("Failed to set audio input on track.");
    return -1;
  }
  
  // TODO - Set track properties (volume, pan, etc.)

  // Set callback for end of playback
  track->set_event_callback([](eTrackEvent event) {
    if (event == eTrackEvent::PlaybackFinished) {
      LOG_INFO("Track playback finished.");
      running = false;
    }
  });

  // Start playback
  LOG_INFO("Starting playback...");
  track->play(); // Non-blocking call
  
  if (!track->is_playing()) {
    LOG_ERROR("Failed to start playback.");
    return -1;
  }
  
  std::cout << "Playback started. Press Ctrl+C to stop.\n";

  // Main application loop
  while (running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  LOG_INFO("Shutting down...");
  track->stop(); // Blocking call
  
  // Get and display statistics
  auto stats = track->get_statistics();
  std::cout << "Playback finished:\n" << stats.to_string() << "\n";
  LOG_INFO("Playback statistics:\n", stats.to_string());

  return 0;
}
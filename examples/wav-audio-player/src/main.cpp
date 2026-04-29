#include <iostream>
#include <csignal>
#include <string>
#include <optional>
#include <vector>
#include <thread>
#include <chrono>

#include "trackservice.h"
#include "deviceservice.h"
#include "track.h"
#include "fileservice.h"
#include "cli.h"
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

static const std::vector<Command> commands = {
  Command("--input-file", "-i", "Specify input WAV file", [](const char *arg){
    input_file_path = std::string(arg);
  }),
  Command("--list-audio-devices", "-ld", "List available audio devices", [](const char *){
    auto& device_manager = DeviceService::instance();
    auto audio_devices = device_manager.get_audio_devices();
    std::cout << "Available Audio Devices:\n";
    for (const auto& device : audio_devices) {
      std::cout << "  ID: " << device->get_id() << ", Name: " << device->get_name() << ", (Input Channels: " << device->get_input_channels() << ", Output Channels: " << device->get_output_channels() << ", Preferred Sample Rate: " << device->get_preferred_sample_rate() << " Hz)\n";
    }
    std::exit(0);
  }),
  Command("--set-audio-output", "-o", "Specify audio output device by ID", [](const char *arg){
    unsigned int device_id = std::stoi(arg);
    auto& device_manager = DeviceService::instance();
    auto audio_device = device_manager.get_audio_device(device_id);
    if (audio_device->get_id() != device_id) {
      std::cerr << "Error: No audio device found with ID " << device_id << ".\n";
      std::exit(1);
    }
    std::cout << "Selected Audio Output Device: " << audio_device->to_string() << "\n";
    audio_output_device_id = device_id;
  }),
  Command("--verbose", "-vb", "Enable verbose logging", [](const char *){
    Logger::instance().enable_console_output(true);
  })
};

int main(int argc, char *argv[])
{
  // Setup logging
  Logger::instance().enable_console_output(false);
  Logger::instance().set_log_file("WavAudioPlayer.log");
  set_thread_name("Main");

  // Parse command line arguments
  CLI cli("wav-audio-player", "WAV Audio Player - A simple audio player using the miniaudioengine library that can play .wav files", VERSION_NUMBER, commands);
  cli.parse_command_line_arguments(argc, argv);

  LOG_INFO("Initializing wav-audio-player...");

  // Resource managers
  TrackService &track_manager = TrackService::instance();
  DeviceService &device_manager = DeviceService::instance();
  FileService &file_manager = FileService::instance();

  if (input_file_path.has_value()) {
    if (!file_manager.is_wav_file(input_file_path.value())) {
      LOG_ERROR("Specified input file is not a valid WAV file: ", input_file_path.value());
      return -1;
    }
    LOG_INFO("WAV file to be played: ", input_file_path.value());
  } else {
    LOG_INFO("No input file specified. Exiting.");
    return 0;
  }

  std::signal(SIGINT, [](int) {
    LOG_INFO("SIGINT received, shutting down...");
    running = false;
  });

  // Add one track
  size_t track_id = track_manager.add_track();
  auto track = track_manager.get_track(track_id);
  if (!track) {
    LOG_ERROR("Failed to create track.");
    return -1;
  }

  // Set audio output device
  auto output_device = (audio_output_device_id.has_value()) ? 
    device_manager.get_audio_device(audio_output_device_id.value()) : device_manager.get_default_audio_output_device();
  
  if (output_device) {
    std::cout << "Using Audio Output Device: " << output_device->to_string() << "\n";
    track_manager.set_audio_output_device(output_device);
    LOG_INFO("Set audio output device: ", output_device->to_string());
  } else {
    LOG_ERROR("No audio output device found.");
    return -1;
  }

  // Open WAV file and add as audio input
  auto wav_file = file_manager.read_wav_file(input_file_path.value());
  if (!wav_file.has_value()) {
    LOG_ERROR("Failed to read WAV file: ", input_file_path.value());
    return -1;
  }
  
  std::cout << "Playing WAV file: " << wav_file.value()->to_string() << "\n";
  track->add_audio_input(wav_file.value());
  LOG_INFO("Set WAV file as audio input: ", wav_file.value()->to_string());
  
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
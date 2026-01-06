#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include <optional>
#include <map>

#include <logger.h>
#include <cli.h>
#include <devicemanager.h>
#include <trackmanager.h>
#include <midicontroller.h>

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "midi-device-input"
#endif

#ifndef VERSION
#define VERSION "1.0.0"
#endif

static bool running = false;

static std::optional<unsigned int> midi_input_device_id = std::nullopt;
static std::optional<unsigned int> audio_output_device_id = std::nullopt;

static MinimalAudioEngine::CommandList commands = {
  MinimalAudioEngine::Command(
    "start",
    "-s",
    "Open the MIDI input device and start receiving MIDI messages",
    [](const char *arg){
      LOG_INFO("Starting MIDI Device Input...");
      running = true;
    }
  ),
  MinimalAudioEngine::Command(
    "--list-midi-devices",
    "-lmd",
    "List available MIDI input devices",
    [](const char *arg){
      LOG_INFO("Listing available MIDI input devices...");
      auto midi_devices = MinimalAudioEngine::Control::DeviceManager::instance().get_midi_devices();
      for (const auto& device : midi_devices)
      {
        std::cout << device.to_string() << std::endl;
      }
      std::exit(0);
    }
  ),
  MinimalAudioEngine::Command(
    "--set-midi-input",
    "-i",
    "Set the MIDI input device by ID",
    [](const char *arg){
      if (arg == nullptr)
      {
        LOG_ERROR("No MIDI input device ID provided.");
        return;
      }
      unsigned int device_id = std::stoi(arg);
      auto midi_device = MinimalAudioEngine::Control::DeviceManager::instance().get_midi_device(device_id);
      if (midi_device.id != device_id)
      {
        LOG_ERROR("MIDI input device with ID " + std::to_string(device_id) + " not found.");
        return;
      }
      LOG_INFO("MIDI input device set to: " + midi_device.to_string());
      std::cout << "MIDI input device set to: " << midi_device.to_string() << std::endl;
      midi_input_device_id = device_id;
    }
  ),
  MinimalAudioEngine::Command(
    "--verbose",
    "-vb",
    "Enable verbose logging output",
    [](const char *arg){
      MinimalAudioEngine::Core::Logger::instance().enable_console_output(true);
      LOG_INFO("Verbose logging enabled.");
    }
  )
};

/** This application opens a MIDI keyboard input and plays a simple oscillator */
int main(int argc, char* argv[])
{
  // Setup logger
  MinimalAudioEngine::Core::Logger::instance().set_log_file("midi_device_input.log");
  MinimalAudioEngine::Core::Logger::instance().enable_console_output(false);

  // Setup CLI
  MinimalAudioEngine::CLI cli(
    PROGRAM_NAME,
    "A MIDI input example program using the minimal-audio-engine library.",
    VERSION,
    commands
  );

  cli.parse_command_line_arguments(argc, argv);

  LOG_INFO("MIDI Device Input Example started.");

  // Setup signal handler for graceful shutdown
  std::signal(SIGINT, [](int){
    running = false;
    LOG_INFO("SIGINT received, shutting down...");
  });

  // Add one track
  size_t track_id = MinimalAudioEngine::Control::TrackManager::instance().add_track();
  auto track = MinimalAudioEngine::Control::TrackManager::instance().get_track(track_id);
  if (!track)
  {
    LOG_ERROR("Failed to create track.");
    return -1;
  }

  // Set default MIDI input device if none specified
  auto midi_input_device = (midi_input_device_id.has_value()) ?
    MinimalAudioEngine::Control::DeviceManager::instance().get_midi_device(midi_input_device_id.value()) :
    MinimalAudioEngine::Control::DeviceManager::instance().get_default_midi_input_device();

  if (midi_input_device.has_value())
  {
    track->add_midi_input(midi_input_device.value());
    std::cout << "Using MIDI input device: " << midi_input_device->to_string() << std::endl;
  }
  else 
  {
    LOG_ERROR("No MIDI input device available.");
    return -1;
  }

  // Set MIDI message callback functions
  track->set_midi_note_on_callback([](const MinimalAudioEngine::Control::MidiNoteMessage &message, MinimalAudioEngine::Control::TrackPtr _track)
  {
    MinimalAudioEngine::Data::eMidiNoteValues note_value = static_cast<MinimalAudioEngine::Data::eMidiNoteValues>(message.note_number());
    std::cout << "MIDI Note Off: " << note_value << std::endl; 
  });

  track->set_midi_note_off_callback([](const MinimalAudioEngine::Control::MidiNoteMessage& message, MinimalAudioEngine::Control::TrackPtr _track) {
    MinimalAudioEngine::Data::eMidiNoteValues note_value = static_cast<MinimalAudioEngine::Data::eMidiNoteValues>(message.note_number());
    std::cout << "MIDI Note Off: " << note_value << std::endl;
  });

  track->set_midi_control_change_callback([](const MinimalAudioEngine::Control::MidiControlMessage& message, MinimalAudioEngine::Control::TrackPtr _track) {
    if (message.controller_value() == static_cast<int>(MinimalAudioEngine::Data::eMidiControllerValues::Released))
    {
      // Ignore released events
      return;
    }

    MinimalAudioEngine::Data::eMidiController controller_number = static_cast<MinimalAudioEngine::Data::eMidiController>(message.controller_number());
    MinimalAudioEngine::Data::eMidiControllerValues controller_value = static_cast<MinimalAudioEngine::Data::eMidiControllerValues>(message.controller_value());

    std::cout << "MIDI Control Change: " << controller_number << " Value=" << controller_value << std::endl;
  });

  // Program loop
  while (running)
  {
    // Here would be the MIDI device input handling logic
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}
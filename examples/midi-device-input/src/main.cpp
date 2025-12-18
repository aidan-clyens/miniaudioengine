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
#include <coreengine.h>
#include <midicontroller.h>

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "midi-device-input"
#endif

#ifndef VERSION
#define VERSION "1.0.0"
#endif

static bool running = false;

static std::optional<unsigned int> midi_input_device_id = std::nullopt;

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
    "-lm",
    "List available MIDI input devices",
    [](const char *arg){
      LOG_INFO("Listing available MIDI input devices...");
      auto midi_devices = MinimalAudioEngine::DeviceManager::instance().get_midi_devices();
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
      auto midi_device = MinimalAudioEngine::DeviceManager::instance().get_midi_device(device_id);
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
      MinimalAudioEngine::Logger::instance().enable_console_output(true);
      LOG_INFO("Verbose logging enabled.");
    }
  )
};

static MinimalAudioEngine::MidiControllerActionMap midi_controller_actions = {
  { MinimalAudioEngine::eMidiController::Play, [](int value){ std::cout << "MIDI: Play" <<  std::endl; } },
  { MinimalAudioEngine::eMidiController::Record, [](int value){ std::cout << "MIDI: Record" <<  std::endl; } },
  { MinimalAudioEngine::eMidiController::ModulationWheel, [](int value){ std::cout << "MIDI: Modulation Wheel changed to value " << value << std::endl; } },
  { MinimalAudioEngine::eMidiController::Pot1, [](int value){ std::cout << "MIDI: Pot 1 changed to value " << value << std::endl; } },
  { MinimalAudioEngine::eMidiController::Pot2, [](int value){ std::cout << "MIDI: Pot 2 changed to value " << value << std::endl; } },
  { MinimalAudioEngine::eMidiController::Pot3, [](int value){ std::cout << "MIDI: Pot 3 changed to value " << value << std::endl; } },
  { MinimalAudioEngine::eMidiController::Pot4, [](int value){ std::cout << "MIDI: Pot 4 changed to value " << value << std::endl; } },
  { MinimalAudioEngine::eMidiController::Pot5, [](int value){ std::cout << "MIDI: Pot 5 changed to value " << value << std::endl; } },
  { MinimalAudioEngine::eMidiController::Pot6, [](int value){ std::cout << "MIDI: Pot 6 changed to value " << value << std::endl; } },
  { MinimalAudioEngine::eMidiController::Pot7, [](int value){ std::cout << "MIDI: Pot 7 changed to value " << value << std::endl; } },
  { MinimalAudioEngine::eMidiController::Pot8, [](int value){ std::cout << "MIDI: Pot 8 changed to value " << value << std::endl; } },
  { MinimalAudioEngine::eMidiController::PreviousTrack, [](int value){ std::cout << "MIDI: Previous Track" <<  std::endl; } },
  { MinimalAudioEngine::eMidiController::NextTrack, [](int value){ std::cout << "MIDI: Next Track" <<  std::endl; } },
  { MinimalAudioEngine::eMidiController::Up, [](int value){ std::cout << "MIDI: Up" <<  std::endl; } },
  { MinimalAudioEngine::eMidiController::Down, [](int value){ std::cout << "MIDI: Down" <<  std::endl; } }
};

int main(int argc, char* argv[])
{
  // Setup logger
  MinimalAudioEngine::Logger::instance().set_log_file("midi_device_input.log");
  MinimalAudioEngine::Logger::instance().enable_console_output(false);

  // Setup CLI
  MinimalAudioEngine::CLI cli(
    PROGRAM_NAME,
    "A MIDI input example program using the minimal-audio-engine library.",
    VERSION,
    commands
  );

  cli.parse_command_line_arguments(argc, argv);

  // Setup coreengine
  MinimalAudioEngine::CoreEngine engine;
  engine.start_thread();

  LOG_INFO("MIDI Device Input Example started.");

  // Setup signal handler for graceful shutdown
  std::signal(SIGINT, [](int){
    running = false;
    LOG_INFO("SIGINT received, shutting down...");
  });

  // Add one track
  size_t track_id = MinimalAudioEngine::TrackManager::instance().add_track();
  auto track = MinimalAudioEngine::TrackManager::instance().get_track(track_id);
  if (!track)
  {
    LOG_ERROR("Failed to create track.");
    return -1;
  }

  // Set default MIDI input device if none specified
  auto midi_input_device = (midi_input_device_id.has_value()) ?
    MinimalAudioEngine::DeviceManager::instance().get_midi_device(midi_input_device_id.value()) :
    MinimalAudioEngine::DeviceManager::instance().get_default_midi_input_device();

  if (midi_input_device.has_value())
  {
    track->add_midi_device_input(midi_input_device.value());
    std::cout << "Using MIDI input device: " << midi_input_device->to_string() << std::endl;
  }
  else 
  {
    LOG_ERROR("No MIDI input device available.");
    return -1;
  }

  // Set callback for end of playback
  track->set_event_callback([](MinimalAudioEngine::eTrackEvent event) {
    if (event == MinimalAudioEngine::eTrackEvent::PlaybackFinished) {
      LOG_INFO("Track playback finished.");
      running = false;
    }
  });

  // Set MIDI message callback functions
  track->set_midi_note_on_callback([](const MinimalAudioEngine::MidiNoteMessage& message, MinimalAudioEngine::TrackPtr _track) {
    std::cout << "MIDI Note On received: " << message.to_string() << std::endl;
  });
  track->set_midi_note_off_callback([](const MinimalAudioEngine::MidiNoteMessage& message, MinimalAudioEngine::TrackPtr _track) {
    std::cout << "MIDI Note Off received: " << message.to_string() << std::endl;
  });
  track->set_midi_control_change_callback([](const MinimalAudioEngine::MidiControlMessage& message, MinimalAudioEngine::TrackPtr _track) {
    if (message.controller_value() == static_cast<int>(MinimalAudioEngine::eMidiControllerValues::Released))
    {
      // Ignore released events
      return;
    }

    auto it = midi_controller_actions.find(static_cast<MinimalAudioEngine::eMidiController>(message.controller_number()));
    if (it != midi_controller_actions.end()) {
      it->second(static_cast<int>(message.controller_value()));
    }
    else
    {
      std::cout << "MIDI Control Change received: " << message.to_string() << std::endl;
    }
  });

  // Program loop
  while (running)
  {
    // Here would be the MIDI device input handling logic
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  engine.stop_thread();

  return 0;
}
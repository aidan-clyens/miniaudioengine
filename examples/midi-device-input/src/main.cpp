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
#include <audioprocessor.h>

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "midi-device-input"
#endif

#ifndef VERSION
#define VERSION "1.0.0"
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/** @class OscillatorAudioProcessor 
 *  @brief Simple audio processor that generates a sine wave oscillator.
 */
class OscillatorAudioProcessor : public MinimalAudioEngine::IAudioProcessor
{
public:
  OscillatorAudioProcessor() = default;
  ~OscillatorAudioProcessor() override = default;

  void set_frequency(const float frequency)
  {
    m_frequency = frequency;
  }

  void get_next_audio_frame(float *output_buffer, unsigned int frames, unsigned int channels, unsigned int sample_rate) override
  {
    LOG_INFO("Generating oscillator audio frame: " +
             std::to_string(frames) + " frames, " +
             std::to_string(channels) + " channels, " +
             std::to_string(sample_rate) + " Hz");

    for (unsigned int i = 0; i < frames; ++i)
    {
      float sample = static_cast<float>(std::sin(m_phase));
      m_phase += (2.0 * M_PI * m_frequency) / static_cast<float>(sample_rate);
      if (m_phase >= (2.0 * M_PI))
      {
        m_phase -= (2.0 * M_PI);
      }

      for (unsigned int ch = 0; ch < channels; ++ch)
      {
        output_buffer[i * channels + ch] = sample;
      }
    }
  }

  std::string to_string() const override
  {
    return "OscillatorAudioProcessor(Frequency=" + std::to_string(m_frequency) + " Hz)";
  }

private:
  float m_frequency = 440.0f; // A4
  float m_phase = 0.0f;
};

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
      auto midi_devices = MinimalAudioEngine::DeviceManager::instance().get_midi_devices();
      for (const auto& device : midi_devices)
      {
        std::cout << device.to_string() << std::endl;
      }
      std::exit(0);
    }
  ),
  MinimalAudioEngine::Command(
    "--list-audio-devices",
    "-lad",
    "List available Audio output devices",
    [](const char *arg){
      LOG_INFO("Listing available Audio output devices...");
      auto audio_devices = MinimalAudioEngine::DeviceManager::instance().get_audio_devices();
      for (const auto& device : audio_devices)
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
    "--set-audio-output",
    "-o",
    "Set the Audio output device by ID",
    [](const char *arg){
      if (arg == nullptr)
      {
        LOG_ERROR("No Audio output device ID provided.");
        return;
      }
      unsigned int device_id = std::stoi(arg);
      auto audio_device = MinimalAudioEngine::DeviceManager::instance().get_audio_device(device_id);
      if (audio_device.id != device_id)
      {
        LOG_ERROR("Audio output device with ID " + std::to_string(device_id) + " not found.");
        return;
      }
      LOG_INFO("Audio output device set to: " + audio_device.to_string());
      std::cout << "Audio output device set to: " << audio_device.to_string() << std::endl;
      audio_output_device_id = device_id;
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

  // Set default Audio output device if none specified
  auto audio_output_device = (audio_output_device_id.has_value()) ?
    MinimalAudioEngine::DeviceManager::instance().get_audio_device(audio_output_device_id.value()) :
    MinimalAudioEngine::DeviceManager::instance().get_default_audio_output_device();

  if (audio_output_device.has_value())
  {
    track->add_audio_device_output(audio_output_device.value());
    std::cout << "Using Audio output device: " << audio_output_device->to_string() << std::endl;
  }
  else 
  {
    LOG_ERROR("No Audio output device available.");
    return -1;
  }

  // Add an oscillator audio processor to the track
  auto oscillator_processor = std::make_shared<OscillatorAudioProcessor>();
  track->add_audio_processor(oscillator_processor);

  // Set callback for end of playback
  track->set_event_callback([](MinimalAudioEngine::eTrackEvent event) {
    if (event == MinimalAudioEngine::eTrackEvent::PlaybackFinished) {
      LOG_INFO("Track playback finished.");
      running = false;
    }
  });

  // Set MIDI message callback functions
  track->set_midi_note_on_callback([](const MinimalAudioEngine::MidiNoteMessage& message, MinimalAudioEngine::TrackPtr _track) {
    int note_value = message.note_number();
    std::string note_name = MinimalAudioEngine::get_midi_note_name(static_cast<MinimalAudioEngine::eMidiNoteValues>(note_value));
    std::cout << "MIDI Note On: " << note_name << std::endl;
  });
  track->set_midi_note_off_callback([](const MinimalAudioEngine::MidiNoteMessage& message, MinimalAudioEngine::TrackPtr _track) {
    int note_value = message.note_number();
    std::string note_name = MinimalAudioEngine::get_midi_note_name(static_cast<MinimalAudioEngine::eMidiNoteValues>(note_value));
    std::cout << "MIDI Note Off: " << note_name << std::endl;
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
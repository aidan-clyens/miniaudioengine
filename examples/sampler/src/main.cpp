#include <iostream>
#include <csignal>
#include <string>
#include <filesystem>
#include <map>
#include <functional>

#include "logger.h"
#include "trackmanager.h"
#include "devicemanager.h"
#include "filemanager.h"
#include "sample.h"
#include "midicontroltypes.h"
#include "sampleplayer.h"
#include "cli.h"
#include "mididevice.h"

static const std::filesystem::path SAMPLE_FOLDER = "C:\\Projects\\miniaudioengine\\samples\\drums";

using namespace miniaudioengine;

namespace sampler
{

static std::atomic<bool> g_running{false};

/** @class Sampler
 *  @brief A simple audio sampler application that loads WAV samples and maps them to MIDI notes. 
 */
class Sampler
{
public:
  Sampler():
    m_track_manager(control::TrackManager::instance()),
    m_device_manager(control::DeviceManager::instance()),
    m_file_manager(file::FileManager::instance()),
    m_logger(core::Logger::instance())
  {
    // Create main track
    m_track = m_track_manager.create_child_track();

    // Set to default audio output device
    auto default_output = m_device_manager.get_default_audio_output_device();
    if (!default_output)
    {
      LOG_ERROR("Sampler: No default audio output device found.");
      return;
    }
    m_track_manager.set_audio_output_device(*default_output);

    // Set to default MIDI input device
    auto default_midi_input = m_device_manager.get_default_midi_input_device();
    if (!default_midi_input)
    {
      LOG_ERROR("Sampler: No default MIDI input device found.");
      return;
    }
    m_track->add_midi_input(*default_midi_input);

    // Add Sample Player processor to track
    m_sample_player = std::make_shared<processing::SamplePlayer>();
    m_track->add_audio_processor(m_sample_player);
  }

  /** @brief Lists all WAV sample files in the specified directory.
   *  @param directory The directory to search for sample files.
   *  @return A vector of filesystem paths to the WAV sample files.
   */
  std::vector<std::filesystem::path> list_samples(const std::filesystem::path &directory)
  {
    if (!m_file_manager.is_directory(directory))
    {
      LOG_ERROR("Sampler: Specified path is not a directory: ", directory);
      return {};
    }

    auto wav_files = m_file_manager.list_wav_files_in_directory(directory);
    return wav_files;
  }

  std::vector<control::MidiDevice> list_midi_devices()
  {
    return m_device_manager.get_midi_devices();
  }

  std::vector<control::AudioDevice> list_audio_devices()
  {
    return m_device_manager.get_audio_devices();
  }

  /** @brief Adds a sample file and maps it to a specific MIDI note.
   *  @param sample_path The filesystem path to the WAV sample file.
   *  @param note The MIDI note value to map the sample to.
   */
  void add_sample(const std::filesystem::path &sample_path, const data::eMidiNoteValues note)
  {
    if (!m_file_manager.is_wav_file(sample_path))
    {
      LOG_ERROR("Sampler: Sample file is not a valid WAV file: ", sample_path);
      return;
    }

    LOG_INFO("Sampler: Loading WAV file: ", sample_path);
    auto wav_file = m_file_manager.read_wav_file(sample_path);
    if (!wav_file)
    {
      LOG_ERROR("Sampler: Failed to load WAV file: ", sample_path);
      return;
    }

    processing::SamplePtr sample = std::make_shared<processing::Sample>(*wav_file);

    // Add to sample map
    m_sample_player->add_sample(sample, note);
    LOG_INFO("Sampler: Loaded sample: ", *sample);
  }

  void set_audio_output_device(const control::AudioDevice &device)
  {
    try
    {
      m_track_manager.set_audio_output_device(device);
    } catch (const std::exception &e)
    {
      LOG_ERROR("Sampler: Failed to set audio output device: ", e.what());
    }
  }

  void set_midi_input_device(const control::MidiDevice &device)
  {
    try
    {
      m_track->add_midi_input(device);
    } catch (const std::exception &e)
    {
      LOG_ERROR("Sampler: Failed to set MIDI input device: ", e.what());
    }
  }

  /** @brief Starts the audio processing and MIDI handling.
   */
  void run()
  {
    g_running.store(true);
    LOG_INFO("Sampler: Starting audio processing...");

    if (!m_track->play())
    {
      std::cerr << "Error: Failed to start audio processing." << std::endl;
      return;
    }

    // Main loop
    while (g_running.load())
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    m_track->stop();

    LOG_INFO("Sampler: Stopped audio processing.");
  }

  /** @brief Enables or disables verbose logging output.
   *  @param enable True to enable verbose logging, false to disable.
   */
  void enable_verbose_logging(bool enable)
  {
    m_logger.enable_console_output(enable);
  }

private:
  control::TrackManager &m_track_manager;
  control::DeviceManager &m_device_manager;
  file::FileManager &m_file_manager;

  control::TrackPtr m_track;
  processing::SamplePlayerPtr m_sample_player;

  core::Logger &m_logger;
};

} // namespace sampler

/** @brief Main entry point for the sampler application.
 *  @return Exit code.
 */
int main()
{
  // Register exit signal handler (Ctrl+C)
  std::signal(SIGINT, [](int)
  {
    sampler::g_running.store(false);
  });

  core::Logger &logger = core::Logger::instance();
  logger.enable_console_output(false);
  logger.set_log_file("Sampler.log");
  core::set_thread_name("Sampler");

  sampler::Sampler app;

  CommandList commands = {
    Command("--list-samples", "-ls", "List all WAV sample files in the sample directory", [&app](const char *)
    {
      auto samples = app.list_samples(SAMPLE_FOLDER);
      std::cout << "WAV Sample Files in " << SAMPLE_FOLDER << ":\n";
      for (const auto &sample : samples)
      {
        std::cout << sample << "\n";
      }
      std::exit(0);
    }),
    Command("--list-midi-devices", "-lm", "List all available MIDI input devices", [&app](const char *)
    {
      auto midi_inputs = app.list_midi_devices();
      std::cout << "Available MIDI Input Devices:\n";
      for (const auto &device : midi_inputs)
      {
        std::cout << device.to_string() << "\n";
      }
      std::exit(0);
    }),
    Command("--list-audio-devices", "-la", "List all available audio output devices", [&app](const char *)
    {
      auto audio_outputs = app.list_audio_devices();
      std::cout << "Available Audio Output Devices:\n";
      for (const auto &device : audio_outputs)
      {
        std::cout << device.to_string() << "\n";
      }
      std::exit(0);
    }),
    Command("--input", "-i", "Specify MIDI input device ID", [&app](const char *arg)
    {
      unsigned int device_id = static_cast<unsigned int>(std::stoi(arg));
      auto midi_inputs = app.list_midi_devices();
      auto it = std::find_if(midi_inputs.begin(), midi_inputs.end(), [device_id](const control::MidiDevice &device)
      {
        return device.id == device_id;
      });
      if (it != midi_inputs.end())
      {
        std::cout << "Using MIDI input device: " << it->to_string() << "\n";
        app.set_midi_input_device(*it);
      }
      else
      {
        std::cerr << "MIDI input device with ID " << device_id << " not found." << std::endl;
        std::exit(1);
      }
    }),
    Command("--output", "-o", "Specify audio output device ID", [&app](const char *arg)
    {
      unsigned int device_id = static_cast<unsigned int>(std::stoi(arg));
      auto audio_outputs = app.list_audio_devices();
      auto it = std::find_if(audio_outputs.begin(), audio_outputs.end(), [device_id](const control::AudioDevice &device)
      {
        return device.id == device_id;
      });
      if (it != audio_outputs.end())
      {
        std::cout << "Using audio output device: " << it->to_string() << "\n";
        app.set_audio_output_device(*it);
      }
      else
      {
        std::cerr << "Audio output device with ID " << device_id << " not found." << std::endl;
        std::exit(1);
      }
    }),
    Command("--verbose", "-vb", "Enable verbose logging output", [&app](const char *)
    {
      app.enable_verbose_logging(true);
    })
  };

  CLI cli("MiniAudioEngine Sampler", "A simple audio sampler application.", "1.0.0", commands);
  cli.parse_command_line_arguments(__argc, __argv);

  auto wav_files = app.list_samples(SAMPLE_FOLDER);
  data::eMidiNoteValues note = data::eMidiNoteValues::C_4;
  for (const auto &file : wav_files)
  {
    std::cout << "Mapping sample file: " << file.filename() << " to " << note << std::endl;
    app.add_sample(file, note);
    note = static_cast<data::eMidiNoteValues>(static_cast<int>(note) + 1);
  }

  std::cout << std::endl;
  std::cout << "Starting sampler application. Press Ctrl+C to exit." << std::endl;

  app.run();

  return 0;
}
#include <iostream>
#include <csignal>
#include <string>
#include <thread>
#include <chrono>
#include <filesystem>
#include <functional>
#include <optional>

#include <CLI11.hpp>

#include "audiosession.h"
#include "track.h"
#include "file.h"
#include "logger.h"

#include "clock.hpp"
#include "sequencer.hpp"

constexpr const char *APP_NAME = "drum-sampler";
constexpr const char *APP_DESCRIPTION = "Drum Sampler - Electronic drum kit sampler";
constexpr const char *APP_LOG_FILEPATH = "drum-sampler.log";

constexpr const char *ARG_VERBOSE = "--verbose";

static std::filesystem::path samples_path;
static bool running = false;

enum eType
{
  KICK,
  SNARE,
  OPEN_HAT,
  CLOSED_HAT,
  CYMBAL
};

class Sampler : public ISequencerObservable
{
  using BeatToSampleMap = std::map<unsigned int, eType>;

public:
  Sampler()
  {
    p_audio_device = m_session.get_default_audio_output_device();
    p_clock = std::make_shared<Clock>();
    p_sequencer = std::make_shared<Sequencer>();
  }

  virtual ~Sampler() = default;

  void load_sample(const eType type, const std::filesystem::path &file_path)
  {
    LOG_INFO("Sampler: load_sample - Add ", type, " sample: ", file_path);

    miniaudioengine::TrackPtr track = m_session.add_track();
    miniaudioengine::FilePtr file = m_session.get_audio_file(file_path);
    if (file == nullptr)
    {
      LOG_WARNING("Sample: load_sample: Failed to open sample file at ", file_path);
      return;
    }

    track->add_audio_input(file);
    track->add_audio_output(p_audio_device);

    m_tracks[type] = track;
  }

  void play_sample(const eType type)
  {
    LOG_INFO("Sampler: play_sample - Play ", type);

    if (!m_tracks.contains(type))
    {
      LOG_WARNING("Sampler: play_sample - No sample of type ", type);
      return;
    }

    miniaudioengine::TrackPtr track = m_tracks[type];
    if (track->is_playing())
    {
      track->stop();
    }
    if (!track->play())
    {
      LOG_WARNING("Sampler: play_sample - Failed to play ", type, " sample");
    }
  }

  void set_sample_to_beat(const eType sample_type, const unsigned int beat_num)
  {
    m_sequencer_sample_map[beat_num] = sample_type;
  }

  void clear_sample_from_beat(const eType sample_type, const unsigned int beat_num)
  {
    if (m_sequencer_sample_map.contains(beat_num))
    {
      m_sequencer_sample_map.erase(beat_num);
    }
  }

  void start_sequencer(const unsigned int bpm)
  {
    p_clock->set_bpm(bpm);
    p_clock->register_observer(p_sequencer);
    p_sequencer->register_observer(shared_from_this());

    LOG_INFO("Sampler: start_sequencer - BPM set to ", p_clock->get_bpm());

    p_clock->start();
    p_sequencer->start();
  }

  void stop_sequencer()
  {
    LOG_INFO("Sampler: stop_sequencer");
    p_sequencer->stop();
    p_clock->stop();
  }

  void callback(unsigned int beat_num) override
  {
    if (m_sequencer_sample_map.contains(beat_num))
    {
      eType sample_type = m_sequencer_sample_map.at(beat_num);
      LOG_INFO("Sampler: callback - Beat = ", beat_num, " Sample = ", sample_type);
      play_sample(sample_type);
    }
    else
    {
      LOG_INFO("Sample: sequencer_callback - Beat = ", beat_num);
    }
  }

private:
  miniaudioengine::AudioSession m_session;
  miniaudioengine::DevicePtr p_audio_device;

  std::map<eType, miniaudioengine::TrackPtr> m_tracks;
  ClockPtr p_clock;
  SequencerPtr p_sequencer;
  BeatToSampleMap m_sequencer_sample_map;
};

using SamplerPtr = std::shared_ptr<Sampler>;

/** @brief Parses command line arguments and configures the audio session.
 *  @param argc Argument count from main()
 *  @param argv Argument vector from main()
 *  @return 0 on success, non-zero on failure.
 */
int parse_cli_arguments(int argc, char *argv[])
{
  // Parse command line arguments
  CLI::App app{std::string(APP_NAME) + " - " + APP_DESCRIPTION};
  argv = app.ensure_utf8(argv);

  app.add_option("--samples", samples_path);

  app.add_flag_callback(ARG_VERBOSE, []()
                        { miniaudioengine::framework::Logger::instance().enable_console_output(true); }, "Enable verbose logging");

  CLI11_PARSE(app, argc, argv);
  return 0;
}

/** @brief Main entry point for the WAV audio player application.
 */
int main(int argc, char *argv[])
{
  // Setup logging
  miniaudioengine::framework::Logger::instance().enable_console_output(false);
  miniaudioengine::framework::Logger::instance().set_log_file(APP_LOG_FILEPATH);
  miniaudioengine::framework::set_thread_name("Main");

  if (parse_cli_arguments(argc, argv) != 0)
  {
    return -1;
  }

  LOG_INFO("Initializing ", APP_NAME, "...");

  // Handle SIGINT (Ctrl+C) for graceful shutdown
  std::signal(SIGINT, [](int)
              { running = false; });

  std::cout << "DRUM KIT" << std::endl;
  std::cout << "1 - Kick" << std::endl;
  std::cout << "2 - Snare" << std::endl;
  std::cout << "3 - Open Hat" << std::endl;
  std::cout << "4 - Closed Hat" << std::endl;
  std::cout << "5 - Cymbal" << std::endl;

  SamplerPtr sampler = std::make_shared<Sampler>();

  sampler->load_sample(eType::KICK, samples_path / "kick.wav");
  sampler->load_sample(eType::SNARE, samples_path / "snare.wav");
  sampler->load_sample(eType::OPEN_HAT, samples_path / "open_hat.wav");
  sampler->load_sample(eType::CLOSED_HAT, samples_path / "closed_hat.wav");

  sampler->set_sample_to_beat(eType::KICK, 0);
  sampler->set_sample_to_beat(eType::CLOSED_HAT, 1);
  sampler->set_sample_to_beat(eType::SNARE, 2);
  sampler->set_sample_to_beat(eType::CLOSED_HAT, 3);
  sampler->set_sample_to_beat(eType::KICK, 4);
  sampler->set_sample_to_beat(eType::CLOSED_HAT, 5);
  sampler->set_sample_to_beat(eType::SNARE, 6);
  sampler->set_sample_to_beat(eType::CLOSED_HAT, 7);

  sampler->start_sequencer(DEFAULT_BPM);

  running = true;
  while (running)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  std::cout << "Exiting..." << std::endl;
  sampler->stop_sequencer();

  return 0;
}
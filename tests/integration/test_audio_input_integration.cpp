#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <memory>

#include "audiostreamcontroller.h"
#include "trackmanager.h"
#include "filemanager.h"
#include "wavfile.h"
#include "devicemanager.h"
#include "logger.h"

using namespace miniaudioengine::control;
using namespace miniaudioengine::data;
using namespace miniaudioengine::core;
using namespace miniaudioengine::file;

// Configure test setup and teardown
class AudioInputIntegrationTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    m_track_manager.clear_tracks();
    m_track_manager.set_audio_output_device(
      DeviceManager::instance().get_default_audio_output_device().value()
    );
  }

  void TearDown() override
  {
    m_track_manager.clear_tracks();
  }

  TrackPtr create_track()
  {
    size_t current_track_count = m_track_manager.get_track_count();
    auto track_ptr = m_track_manager.create_child_track();
    size_t new_track_count = m_track_manager.get_track_count();
    EXPECT_EQ(new_track_count, current_track_count + 1);
    return track_ptr;
  }

  WavFilePtr load_wav_file(const std::string &filepath)
  {
    auto wav_file_opt = FileManager::instance().read_wav_file(filepath);
    EXPECT_TRUE(wav_file_opt.has_value());
    return wav_file_opt.value();
  }

  void verify_audio_statistics(AudioOutputStatistics stats, bool is_initial)
  {
    if (is_initial)
    {
      EXPECT_EQ(stats.total_frames_read, 0);
      EXPECT_EQ(stats.total_read_time_ms, 0);
      EXPECT_EQ(stats.total_batches, 0);
      EXPECT_EQ(stats.throughput_frames_per_second, 0);
      return;
    }

    EXPECT_GT(stats.total_frames_read, m_prev_stats.total_frames_read);
    EXPECT_GT(stats.total_read_time_ms, m_prev_stats.total_read_time_ms);
    EXPECT_GT(stats.total_batches, m_prev_stats.total_batches);
    
    EXPECT_GT(stats.throughput_frames_per_second, 45000.0); // Expect at least 45 kHz throughput
    EXPECT_LT(stats.throughput_frames_per_second, 52000.0); // Expect less than 52 kHz throughput

    m_prev_stats = stats;
  }

private:
  TrackManager& m_track_manager = TrackManager::instance();

  AudioOutputStatistics m_prev_stats = {};
};

TEST_F(AudioInputIntegrationTest, AudioFileInput)
{
  set_thread_name("Main");

  // Add a track
  TrackPtr track = create_track();
  EXPECT_NE(track, nullptr);

  auto statistics = track->get_statistics();
  LOG_INFO("Initial - Track Statistics: ", statistics.to_string());
  verify_audio_statistics(statistics.audio_output_stats, true);

  // Open a test WAV file and load it into the track
  std::string test_wav_file = "../../../../samples/test.wav";
  auto wav_file = load_wav_file(test_wav_file);
  LOG_INFO("WAV file loaded: ", wav_file->get_filepath());

  track->add_audio_input(wav_file);

  track->play();
  std::this_thread::sleep_for(std::chrono::seconds(2));

  statistics = track->get_statistics();
  LOG_INFO("Playing - Track Statistics: ", statistics.to_string());
  verify_audio_statistics(statistics.audio_output_stats, false);

  std::this_thread::sleep_for(std::chrono::seconds(2));

  track->stop();

  statistics = track->get_statistics();
  LOG_INFO("Playing - Track Statistics: ", statistics.to_string());
  verify_audio_statistics(statistics.audio_output_stats, false);
}

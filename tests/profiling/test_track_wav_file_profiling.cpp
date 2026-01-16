#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#include "trackmanager.h"
#include "devicemanager.h"
#include "track.h"
#include "audiostreamcontroller.h"
#include "filemanager.h"
#include "wavfile.h"
#include "logger.h"

#define TEST_WAV_FILE_PATH "C:\\Projects\\minimal-audio-engine\\examples\\wav-audio-player\\samples\\test2.wav"

using namespace miniaudioengine;
using namespace miniaudioengine::control;
using namespace miniaudioengine::data;

class TrackWavFileProfilingTest : public ::testing::Test
{
protected:
  TrackPtr test_track;

  void SetUp() override
  {
    TrackManager::instance().clear_tracks();
    TrackManager::instance().add_track();
    test_track = TrackManager::instance().get_track(0);
  }
};

TrackStatistics play_wav_file(TrackPtr test_track, double read_time_multiplier = 1.0, double batch_size_multiplier = 1.0)
{
  // Start playback
  test_track->play(); // Non-blocking call
  EXPECT_TRUE(test_track->is_playing()) << "Track should be playing after play() is called";

  // Wait
  std::this_thread::sleep_for(std::chrono::seconds(5));

  LOG_INFO("Stopping playback after 5 seconds.");
  test_track->stop(); // Blocking call
  EXPECT_FALSE(test_track->is_playing()) << "Track should not be playing after stop() is called";

  // Get statistics
  auto stats = test_track->get_statistics();
  LOG_INFO("Stopped playing:\n", stats.to_string());

  return stats;
}

TEST_F(TrackWavFileProfilingTest, Throughput)
{
  // Set audio output device
  auto output_device = DeviceManager::instance().get_default_audio_output_device();
  EXPECT_TRUE(output_device.has_value()) << "Default output device should be available";  

  AudioStreamController::instance().set_output_device(output_device.value());

  // Add WAV file as audio input
  auto wav_file = FileManager::instance().read_wav_file(TEST_WAV_FILE_PATH);
  EXPECT_TRUE(wav_file.has_value()) << "WAV file should be loaded successfully";

  EXPECT_FALSE(test_track->has_audio_input()) << "Track should not have audio input before adding WAV file";
  test_track->add_audio_input(wav_file.value());
  EXPECT_TRUE(test_track->has_audio_input()) << "Track should have audio input after adding WAV file";

  // Play WAV file and get statistics
  TrackStatistics stats = play_wav_file(test_track);

  // Check throughput
  LOG_INFO("WAV File Throughput: ", stats.audio_output_stats.throughput_frames_per_second, " Hz");
  EXPECT_GT(stats.audio_output_stats.throughput_frames_per_second, 0.0) << "Throughput should be greater than 0 Hz";
}

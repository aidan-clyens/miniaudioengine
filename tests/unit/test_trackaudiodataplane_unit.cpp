#include <gtest/gtest.h>
#include <iostream>

#include "audiodataplane.h"
#include "filemanager.h"
#include "wavfile.h"
#include "logger.h"

#define TEST_WAV_FILE_PATH "C:\\Projects\\minimal-audio-engine\\examples\\wav-audio-player\\samples\\test2.wav"

using namespace miniaudioengine;
using namespace miniaudioengine::control;
using namespace miniaudioengine::data;

class TrackAudioDataPlaneTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    p_track_audio_dataplane = std::make_shared<AudioDataPlane>();
  }

  void TearDown() override
  {
    p_track_audio_dataplane->stop();
  }

  TrackAudioDataPlanePtr &track_audio_dataplane()
  {
    return p_track_audio_dataplane;
  }

private:
  TrackAudioDataPlanePtr p_track_audio_dataplane;
};

/** @brief TrackAudioDataPlaneTest - Setup
 */
TEST_F(TrackAudioDataPlaneTest, Setup)
{
  EXPECT_NE(track_audio_dataplane(), nullptr) << "AudioDataPlane should not be null after creation";
  EXPECT_FALSE(track_audio_dataplane()->get_input_channels() > 0) << "Input channels should be 0 by default";
  EXPECT_FALSE(track_audio_dataplane()->get_output_channels() > 0) << "Output channels should be 0 by default";
  EXPECT_FALSE(track_audio_dataplane()->is_running()) << "AudioDataPlane should not be running by default";
  LOG_INFO("Created AudioDataPlane: ", track_audio_dataplane()->to_string());
}

TEST_F(TrackAudioDataPlaneTest, SetChannels)
{
  track_audio_dataplane()->set_input_channels(1);
  track_audio_dataplane()->set_output_channels(2);

  EXPECT_EQ(track_audio_dataplane()->get_input_channels(), 1) << "Input channels should be set to 1";
  EXPECT_EQ(track_audio_dataplane()->get_output_channels(), 2) << "Output channels should be set to 2";

  LOG_INFO("AudioDataPlane channels set: Input=", track_audio_dataplane()->get_input_channels(),
           ", Output=", track_audio_dataplane()->get_output_channels());
}

TEST_F(TrackAudioDataPlaneTest, PreloadWavFile)
{
  EXPECT_FALSE(track_audio_dataplane()->is_running()) << "AudioDataPlane should not be running before preloading WAV file";

  auto wav_file = FileManager::instance().read_wav_file(TEST_WAV_FILE_PATH);
  ASSERT_TRUE(wav_file.has_value()) << "Failed to read WAV file from path: " << TEST_WAV_FILE_PATH;
  LOG_INFO("Read WAV file: ", wav_file.value()->to_string());

  track_audio_dataplane()->preload_wav_file(wav_file.value());

  LOG_INFO("Completed reading WAV file in AudioDataPlane.");
}

TEST_F(TrackAudioDataPlaneTest, StartStopProcessing)
{
  track_audio_dataplane()->start();
  EXPECT_TRUE(track_audio_dataplane()->is_running()) << "AudioDataPlane should be running after start()";

  track_audio_dataplane()->stop();
  EXPECT_FALSE(track_audio_dataplane()->is_running()) << "AudioDataPlane should not be running after stop()";
}

TEST_F(TrackAudioDataPlaneTest, ProcessAudioAndStatistics)
{
  auto stats = track_audio_dataplane()->get_audio_output_statistics();
  EXPECT_EQ(stats.total_frames_read, 0) << "Total frames read should be 0 initially";

  LOG_INFO("Initial AudioOutputStatistics: ", stats.to_string());

  track_audio_dataplane()->start();
  EXPECT_TRUE(track_audio_dataplane()->is_running()) << "AudioDataPlane should be running after start()";

  // Process a dummy audio buffer
  const unsigned int n_frames = 512;
  std::vector<float> output_buffer(n_frames * 2, 0.0f); // Stereo output
  track_audio_dataplane()->process_audio(output_buffer.data(), nullptr, n_frames, 0.0, 0);

  // Get updated statistics
  stats = track_audio_dataplane()->get_audio_output_statistics();
  EXPECT_EQ(stats.total_frames_read, n_frames) << "Total frames read should equal processed frames";
  EXPECT_EQ(stats.batch_size_frames, n_frames) << "Batch size frames should equal processed frames";
  EXPECT_EQ(stats.total_batches, 1) << "Total batches should be 1 after one processing call";
  LOG_INFO("Updated AudioOutputStatistics after processing: ", stats.to_string());
}

TEST_F(TrackAudioDataPlaneTest, DoNotProcessAudioWhenStopped)
{
  track_audio_dataplane()->stop();
  EXPECT_FALSE(track_audio_dataplane()->is_running()) << "AudioDataPlane should not be running after stop()";

  auto stats = track_audio_dataplane()->get_audio_output_statistics();
  EXPECT_EQ(stats.total_frames_read, 0) << "Total frames read should be 0 initially";

  // Process a dummy audio buffer
  const unsigned int n_frames = 512;
  std::vector<float> output_buffer(n_frames * 2, 0.0f); // Stereo output
  track_audio_dataplane()->process_audio(output_buffer.data(), nullptr, n_frames, 0.0, 0);

  // Get updated statistics
  stats = track_audio_dataplane()->get_audio_output_statistics();
  EXPECT_EQ(stats.total_frames_read, 0) << "Total frames read should remain 0 when stopped";
  EXPECT_EQ(stats.total_batches, 0) << "Total batches should remain 0 when stopped";
  LOG_INFO("AudioOutputStatistics after processing while stopped: ", stats.to_string());
}
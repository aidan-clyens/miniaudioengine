#include <gtest/gtest.h>
#include <iostream>

#include "trackaudiodataplane.h"
#include "filemanager.h"
#include "wavfile.h"
#include "logger.h"

#define TEST_WAV_FILE_PATH "C:\\Projects\\minimal-audio-engine\\examples\\wav-audio-player\\samples\\test2.wav"

using namespace MinimalAudioEngine;

class TrackAudioDataPlaneTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    p_track_audio_dataplane = std::make_shared<TrackAudioDataPlane>();
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
  EXPECT_NE(track_audio_dataplane(), nullptr) << "TrackAudioDataPlane should not be null after creation";
  LOG_INFO("Created TrackAudioDataPlane: ", track_audio_dataplane()->to_string());
}

TEST_F(TrackAudioDataPlaneTest, ReadWavFile)
{
  auto wav_file = FileManager::instance().read_wav_file(TEST_WAV_FILE_PATH);
  ASSERT_TRUE(wav_file.has_value()) << "Failed to read WAV file from path: " << TEST_WAV_FILE_PATH;
  LOG_INFO("Read WAV file: ", wav_file.value()->to_string());

  std::atomic<bool> read_complete = false;

  track_audio_dataplane()->read_wav_file(wav_file.value(), [&read_complete, &wav_file](const AudioOutputStatistics& stats) {
    read_complete.store(true);

    LOG_INFO("WAV file read completed: ", wav_file.value()->to_string());
    LOG_INFO("Statistics: ", stats.to_string());
  });

  // Read data from track audio dataplane's input buffer
  auto input_buffer = track_audio_dataplane()->get_output_buffer();
  std::vector<float> read_samples;
  float sample;

  while (!read_complete.load() || input_buffer->size() > 0)
  {
    if (input_buffer->try_pop(sample))
    {
      read_samples.push_back(sample);
    }
    else
    {
      // No data available, yield to avoid busy waiting
      std::this_thread::yield();
    }
  }

  LOG_INFO("Completed reading WAV file in TrackAudioDataPlane.");
}
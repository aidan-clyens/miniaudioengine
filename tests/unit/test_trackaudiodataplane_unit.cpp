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

TEST_F(TrackAudioDataPlaneTest, SetChannels)
{
  track_audio_dataplane()->set_input_channels(1);
  track_audio_dataplane()->set_output_channels(2);

  EXPECT_EQ(track_audio_dataplane()->get_input_channels(), 1) << "Input channels should be set to 1";
  EXPECT_EQ(track_audio_dataplane()->get_output_channels(), 2) << "Output channels should be set to 2";

  LOG_INFO("TrackAudioDataPlane channels set: Input=", track_audio_dataplane()->get_input_channels(),
           ", Output=", track_audio_dataplane()->get_output_channels());
}

TEST_F(TrackAudioDataPlaneTest, ReadWavFile)
{
  auto wav_file = FileManager::instance().read_wav_file(TEST_WAV_FILE_PATH);
  ASSERT_TRUE(wav_file.has_value()) << "Failed to read WAV file from path: " << TEST_WAV_FILE_PATH;
  LOG_INFO("Read WAV file: ", wav_file.value()->to_string());

  track_audio_dataplane()->read_wav_file(wav_file.value());

  LOG_INFO("Completed reading WAV file in TrackAudioDataPlane.");
}
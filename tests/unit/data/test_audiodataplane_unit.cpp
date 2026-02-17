#include <gtest/gtest.h>
#include <iostream>

#include "audiodataplane.h"
#include "filemanager.h"
#include "wavfile.h"
#include "logger.h"

#define TEST_WAV_FILE_PATH "C:\\Projects\\miniaudioengine\\examples\\wav-audio-player\\samples\\test2.wav"

using namespace miniaudioengine;
using namespace miniaudioengine::data;
using namespace miniaudioengine::file;

class AudioDataPlaneTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    p_audio_dataplane = std::make_shared<AudioDataPlane>();
  }

  void TearDown() override
  {
    p_audio_dataplane->stop();
    p_audio_dataplane.reset();
  }

  AudioDataPlanePtr &audio_dataplane()
  {
    return p_audio_dataplane;
  }

private:
  AudioDataPlanePtr p_audio_dataplane;
};

/** @brief AudioDataPlaneTest - Init
 */
TEST_F(AudioDataPlaneTest, Action1_Init)
{
  EXPECT_NE(audio_dataplane(), nullptr) << "AudioDataPlane should not be null after creation";
  EXPECT_EQ(audio_dataplane()->get_input_channels(), 0) << "Input channels should be 0 by default";
  EXPECT_EQ(audio_dataplane()->get_output_channels(), 0) << "Output channels should be 0 by default";
  EXPECT_FALSE(audio_dataplane()->is_running()) << "AudioDataPlane should not be running by default";
  LOG_INFO("Created AudioDataPlane: ", audio_dataplane()->to_string());
}

TEST_F(AudioDataPlaneTest, Action2_SetChannels)
{
  audio_dataplane()->set_input_channels(1);
  audio_dataplane()->set_output_channels(2);

  EXPECT_EQ(audio_dataplane()->get_input_channels(), 1) << "Input channels should be set to 1";
  EXPECT_EQ(audio_dataplane()->get_output_channels(), 2) << "Output channels should be set to 2";

  LOG_INFO("AudioDataPlane channels set: Input=", audio_dataplane()->get_input_channels(),
           ", Output=", audio_dataplane()->get_output_channels());

  // Set channels to a negative value. It should raise an invalid argument exception.
  EXPECT_THROW(audio_dataplane()->set_input_channels(-1), std::invalid_argument)
      << "Setting input channels to a negative value should throw an exception";
  EXPECT_THROW(audio_dataplane()->set_output_channels(-1), std::invalid_argument)
      << "Setting output channels to a negative value should throw an exception";
}

TEST_F(AudioDataPlaneTest, Action3_PreloadWavFile)
{
  EXPECT_FALSE(audio_dataplane()->is_running()) << "AudioDataPlane should not be running before preloading WAV file";

  auto wav_file = FileManager::instance().read_wav_file(TEST_WAV_FILE_PATH);
  ASSERT_TRUE(wav_file.has_value()) << "Failed to read WAV file from path: " << TEST_WAV_FILE_PATH;
  LOG_INFO("Read WAV file: ", wav_file.value()->to_string());

  audio_dataplane()->preload_wav_file(wav_file.value());

  LOG_INFO("Completed reading WAV file in AudioDataPlane.");
}

TEST_F(AudioDataPlaneTest, Action4_StartStopProcessing)
{
  audio_dataplane()->start();
  EXPECT_TRUE(audio_dataplane()->is_running()) << "AudioDataPlane should be running after start()";

  audio_dataplane()->stop();
  EXPECT_FALSE(audio_dataplane()->is_running()) << "AudioDataPlane should not be running after stop()";
}

TEST_F(AudioDataPlaneTest, Action5_ProcessAudioAndStatistics)
{
  auto stats = audio_dataplane()->get_audio_output_statistics();
  EXPECT_EQ(stats.total_frames_read, 0) << "Total frames read should be 0 initially";

  LOG_INFO("Initial AudioOutputStatistics: ", stats.to_string());

  audio_dataplane()->start();
  EXPECT_TRUE(audio_dataplane()->is_running()) << "AudioDataPlane should be running after start()";

  // Process a dummy audio buffer
  const unsigned int n_frames = 512;
  std::vector<float> output_buffer(n_frames * 2, 0.0f); // Stereo output
  audio_dataplane()->process_audio(output_buffer.data(), nullptr, n_frames, 0.0, 0);

  // Get updated statistics
  stats = audio_dataplane()->get_audio_output_statistics();
  EXPECT_EQ(stats.total_frames_read, n_frames) << "Total frames read should equal processed frames";
  EXPECT_EQ(stats.batch_size_frames, n_frames) << "Batch size frames should equal processed frames";
  EXPECT_EQ(stats.total_batches, 1) << "Total batches should be 1 after one processing call";
  LOG_INFO("Updated AudioOutputStatistics after processing: ", stats.to_string());
}

TEST_F(AudioDataPlaneTest, Action6_DoNotProcessAudioWhenStopped)
{
  audio_dataplane()->stop();
  EXPECT_FALSE(audio_dataplane()->is_running()) << "AudioDataPlane should not be running after stop()";

  auto stats = audio_dataplane()->get_audio_output_statistics();
  EXPECT_EQ(stats.total_frames_read, 0) << "Total frames read should be 0 initially";

  // Process a dummy audio buffer
  const unsigned int n_frames = 512;
  std::vector<float> output_buffer(n_frames * 2, 0.0f); // Stereo output
  audio_dataplane()->process_audio(output_buffer.data(), nullptr, n_frames, 0.0, 0);

  // Get updated statistics
  stats = audio_dataplane()->get_audio_output_statistics();
  EXPECT_EQ(stats.total_frames_read, 0) << "Total frames read should remain 0 when stopped";
  EXPECT_EQ(stats.total_batches, 0) << "Total batches should remain 0 when stopped";
  LOG_INFO("AudioOutputStatistics after processing while stopped: ", stats.to_string());
}
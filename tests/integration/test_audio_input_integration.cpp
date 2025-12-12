#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <memory>

#include "audioengine.h"
#include "trackmanager.h"
#include "filemanager.h"
#include "wavfile.h"
#include "devicemanager.h"
#include "logger.h"

using namespace MinimalAudioEngine;
using namespace MinimalAudioEngine;
using namespace MinimalAudioEngine;
using namespace MinimalAudioEngine;

TEST(AudioInputIntegrationTest, AudioInput)
{
  set_thread_name("Main");

  // Start the audio engine
  AudioEngine::instance().start_thread();

  // Add a track
  ASSERT_EQ(TrackManager::instance().get_track_count(), 0);
  size_t track_index = TrackManager::instance().add_track();
  ASSERT_EQ(TrackManager::instance().get_track_count(), 1);

  auto track = TrackManager::instance().get_track(track_index);

  LOG_INFO("Track added with index: ", track_index);

  AudioEngineStatistics stats = AudioEngine::instance().get_statistics();
  LOG_INFO("Tracks playing: ", stats.tracks_playing);
  LOG_INFO("Total frames processed: ", stats.total_frames_processed);

  // Open a test WAV file and load it into the track
  std::string test_wav_file = "samples/test.wav";

  auto wav_file = FileManager::instance().read_wav_file(test_wav_file);
  ASSERT_EQ(wav_file->get_filepath(), FileManager::instance().convert_to_absolute(test_wav_file));
  ASSERT_EQ(wav_file->get_filename(), FileManager::instance().convert_to_absolute(test_wav_file).filename().string());

  LOG_INFO("WAV file loaded: ", wav_file->get_filepath());

  track->add_audio_file_input(wav_file);

  // Add audio output to track
  track->add_audio_output();

  track->play();
  std::this_thread::sleep_for(std::chrono::seconds(2));

  // Read audio engine statistics after adding the track and WAV file
  stats = AudioEngine::instance().get_statistics();
  LOG_INFO("Tracks playing: ", stats.tracks_playing);
  LOG_INFO("Total frames processed: ", stats.total_frames_processed);

  std::this_thread::sleep_for(std::chrono::seconds(2));

  track->stop();

  // Wait until AudioEngine stops streaming
  while (AudioEngine::instance().get_state() != eAudioEngineState::Idle)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  // Read audio engine statistics after stopping
  stats = AudioEngine::instance().get_statistics();
  LOG_INFO("Tracks playing: ", stats.tracks_playing);
  LOG_INFO("Total frames processed: ", stats.total_frames_processed);

  // Stop the audio engine
  AudioEngine::instance().stop_thread();
}

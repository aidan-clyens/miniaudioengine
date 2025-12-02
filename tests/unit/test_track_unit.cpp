#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#include "trackmanager.h"
#include "track.h"
#include "audioengine.h"
#include "filemanager.h"
#include "wavfile.h"

using namespace Tracks;

TEST(TrackTest, Setup)
{
  TrackManager::instance().clear_tracks();

  // Create a new track
  size_t index = TrackManager::instance().add_track();
  auto track = TrackManager::instance().get_track(index);

  EXPECT_NE(track, nullptr) << "Track should not be null after creation";
  EXPECT_EQ(TrackManager::instance().get_track_count(), 1) << "Track count should be 1 after adding a track";
}

/** @brief Track - Add Audio Input
 */
TEST(TrackTest, AddAudioInput)
{
  auto track = TrackManager::instance().get_track(0);

  // Add audio input to the track
  track->add_audio_input();

  // Verify the track has an audio input
  EXPECT_TRUE(track->has_audio_input());
  EXPECT_EQ(track->get_audio_input(), 0);
}

/** @brief Track - Add MIDI Input
 */
TEST(TrackTest, AddMidiInput)
{
  auto track = TrackManager::instance().get_track(0);

  // Add MIDI input to the track
  track->add_midi_input();

  // Verify the track has a MIDI input
  EXPECT_TRUE(track->has_midi_input());
  EXPECT_EQ(track->get_midi_input_id(), 0);
}

/** @brief Track - Add Audio Output
 */
TEST(TrackTest, AddAudioOutput)
{
  auto track = TrackManager::instance().get_track(0);

  // Add audio output to the track
  track->add_audio_output();

  // Verify the track has an audio output
  EXPECT_TRUE(track->has_audio_output());
  EXPECT_EQ(track->get_audio_output(), 0);
}

/** @brief Track - Add WAV File Input
 */
TEST(TrackTest, AddWavFileInput)
{
  auto track = TrackManager::instance().get_track(0);

  // Open a test WAV file and load it into the track
  std::string test_wav_file = "samples/test.wav";

  std::shared_ptr<Files::WavFile> wav_file = Files::FileManager::instance().read_wav_file(test_wav_file);
  track->add_audio_file_input(wav_file);
}
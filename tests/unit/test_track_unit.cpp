#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#include "devicemanager.h"
#include "trackmanager.h"
#include "track.h"
#include "audiostreamcontroller.h"
#include "filemanager.h"
#include "wavfile.h"
#include "logger.h"

#define TEST_WAV_FILE_PATH "C:\\Projects\\minimal-audio-engine\\samples\\test.wav"

using namespace MinimalAudioEngine;

/** @brief Track - Setup
 */
TEST(TrackTest, Setup)
{
  TrackManager::instance().clear_tracks();

  // Create a new track
  size_t index = TrackManager::instance().add_track();
  auto track = TrackManager::instance().get_track(index);

  EXPECT_NE(track, nullptr) << "Track should not be null after creation";
  EXPECT_EQ(TrackManager::instance().get_track_count(), 1) << "Track count should be 1 after adding a track";

  LOG_INFO("Created track ", index, ": ", track->to_string());

  EXPECT_FALSE(track->has_audio_input()) << "Track should not have audio input initially";
  EXPECT_FALSE(track->has_midi_input()) << "Track should not have MIDI input initially";
  EXPECT_FALSE(track->has_midi_output()) << "Track should not have MIDI output initially";
}

/** @brief Track - Add Audio Input Device
 */
TEST(TrackTest, AddAudioInputDevice)
{
  auto track = TrackManager::instance().get_track(0);

  // Find a valid audio input device
  auto device = MinimalAudioEngine::DeviceManager::instance().get_default_audio_input_device();
  EXPECT_TRUE(device.has_value()) << "No audio input device found for testing";
  LOG_INFO("Adding audio input device: " + device->to_string());

  // Add audio input to the track
  track->add_audio_input(device.value());
  LOG_INFO("Updated track 0: ", track->to_string());

  // Verify the track has an audio input
  EXPECT_TRUE(track->has_audio_input());
  EXPECT_EQ(std::get<MinimalAudioEngine::AudioDevice>(track->get_audio_input()), device.value());
}

/** @brief Track - Remove Audio Input Device
 */
TEST(TrackTest, RemoveAudioInputDevice)
{
  auto track = TrackManager::instance().get_track(0);

  // Verify the track has an audio input
  EXPECT_TRUE(track->has_audio_input()) << "Track should have audio input before removal";

  // Remove audio input
  track->remove_audio_input();
  LOG_INFO("Removed audio input from track 0: ", track->to_string());

  // Verify the track no longer has an audio input
  EXPECT_FALSE(track->has_audio_input()) << "Track should not have audio input after removal";
}

/** @brief Track - Add Audio Input File
 */
TEST(TrackTest, AddAudioInputFile)
{
  auto track = TrackManager::instance().get_track(0);

  // Find a valid audio input device
  auto file = MinimalAudioEngine::FileManager::instance().read_wav_file(TEST_WAV_FILE_PATH);
  EXPECT_TRUE(file.has_value()) << "Failed to read WAV file for testing";
  LOG_INFO("Adding audio input file: ", file.value()->to_string());

  // Add audio input to the track
  track->add_audio_input(file.value());
  LOG_INFO("Updated track 0: ", track->to_string());

  // Verify the track has an audio input
  EXPECT_TRUE(track->has_audio_input());
  EXPECT_EQ(std::get<MinimalAudioEngine::WavFilePtr>(track->get_audio_input()), file.value());
}

/** @brief Track - Remove Audio Input File
 */
TEST(TrackTest, RemoveAudioInputFile)
{
  auto track = TrackManager::instance().get_track(0);

  // Verify the track has an audio input
  EXPECT_TRUE(track->has_audio_input()) << "Track should have audio input before removal";

  // Remove audio input
  track->remove_audio_input();
  LOG_INFO("Removed audio input from track 0: ", track->to_string());

  // Verify the track no longer has an audio input
  EXPECT_FALSE(track->has_audio_input()) << "Track should not have audio input after removal";
}

/** @brief Track - Add Audio Input with Invalid Device
 */
TEST(TrackTest, AddAudioInput_InvalidDevice)
{
  auto track = TrackManager::instance().get_track(0);

  // Get an output device to trigger invalid input
  auto output_device = MinimalAudioEngine::DeviceManager::instance().get_default_audio_output_device();
  try
  {
    track->add_audio_input(output_device.value());
    FAIL() << "Expected std::runtime_error exception for invalid device ID";
  }
  catch (const std::runtime_error& e)
  {
    SUCCEED();
  }
  catch (...)
  {
    FAIL() << "Expected std::runtime_error exception for invalid device ID";
  }

  // Verify the track does not have an audio input
  EXPECT_FALSE(track->has_audio_input());
}

/** @brief Track - Add MIDI Input Device
 */
TEST(TrackTest, AddMidiInputDevice)
{
  auto track = TrackManager::instance().get_track(0);

  // Find a valid MIDI input device
  std::optional<MinimalAudioEngine::MidiDevice> device = MinimalAudioEngine::DeviceManager::instance().get_default_midi_input_device();
  EXPECT_TRUE(device.has_value()) << "No MIDI input device found for testing";
  LOG_INFO("Adding MIDI input device: " + device->to_string());

  // Add MIDI input to the track
  track->add_midi_input(device.value());
  LOG_INFO("Updated track 0: ", track->to_string());

  // Verify the track has a MIDI input
  EXPECT_TRUE(track->has_midi_input());
  EXPECT_EQ(std::get<MinimalAudioEngine::MidiDevice>(track->get_midi_input()), device.value());
}

/** @brief Track - Remove MIDI Input Device
 */
TEST(TrackTest, RemoveMidiInputDevice)
{
  auto track = TrackManager::instance().get_track(0);

  // Verify the track has a MIDI input
  EXPECT_TRUE(track->has_midi_input()) << "Track should have MIDI input before removal";

  // Remove MIDI input
  track->remove_midi_input();
  LOG_INFO("Removed MIDI input from track 0: ", track->to_string());

  // Verify the track no longer has a MIDI input
  EXPECT_FALSE(track->has_midi_input()) << "Track should not have MIDI input after removal";
}

/** @brief Track - Add MIDI Output
 */
TEST(TrackTest, AddMidiOutput) {
  auto track = TrackManager::instance().get_track(0);

  // Find a valid MIDI output device
  std::optional<MinimalAudioEngine::MidiDevice> device = MinimalAudioEngine::DeviceManager::instance().get_default_midi_output_device();
  EXPECT_TRUE(device.has_value()) << "No MIDI output device found for testing";
  LOG_INFO("Adding MIDI output device: " + device->to_string());

  // Add MIDI output to the track
  track->add_midi_output(device.value());
  LOG_INFO("Updated track 0: ", track->to_string());

  // Verify the track has a MIDI output
  EXPECT_TRUE(track->has_midi_output());
  EXPECT_EQ(std::get<MinimalAudioEngine::MidiDevice>(track->get_midi_output()), device.value());
}

/** @brief Track - Remove MIDI Output 
 */
TEST(TrackTest, RemoveMidiOutput)
{
  auto track = TrackManager::instance().get_track(0);

  // Verify the track has a MIDI output
  EXPECT_TRUE(track->has_midi_output()) << "Track should have MIDI output before removal";

  // Remove MIDI output
  track->remove_midi_output();
  LOG_INFO("Removed MIDI output from track 0: ", track->to_string());

  // Verify the track no longer has a MIDI output
  EXPECT_FALSE(track->has_midi_output()) << "Track should not have MIDI output after removal";
}

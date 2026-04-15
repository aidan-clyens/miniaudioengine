#include <gtest/gtest.h>
#include <iostream>
#include <memory>

// miniaudioengine::core
#include "logger.h"
// miniaudioengine::control
#include "miniaudioengine/trackservice.h"
#include "miniaudioengine/deviceservice.h"
#include "miniaudioengine/track.h"
#include "audiocontroller.h"
#include "miniaudioengine/fileservice.h"

// Mocks
#include "device_mock.h"

#define TEST_WAV_FILE_PATH "C:\\Projects\\miniaudioengine\\samples\\test.wav"

using namespace miniaudioengine;
using namespace miniaudioengine::core;
using namespace miniaudioengine::test;

class TrackTest : public ::testing::Test
{
protected:
  TrackPtr test_track;
  DeviceHandlePtr mock_output_device;

  void SetUp() override
  {
    TrackService::instance().clear_tracks();
    TrackService::instance().add_track();
    test_track = TrackService::instance().get_track(0);

    mock_output_device = miniaudioengine::test::make_mock_audio_output_device();
    TrackService::instance().set_audio_output_device(mock_output_device);
  }

  void TearDown() override
  {
    if (test_track && test_track->is_playing())
    {
      test_track->stop();
    }
  }
};

/** @brief Track - Setup
 */
TEST_F(TrackTest, Action1_Setup)
{
  EXPECT_NE(test_track, nullptr) << "Track should not be null after creation";

  EXPECT_FALSE(test_track->has_audio_input()) << "Track should not have audio input initially";
  EXPECT_FALSE(test_track->has_midi_input()) << "Track should not have MIDI input initially";
  EXPECT_FALSE(test_track->has_midi_output()) << "Track should not have MIDI output initially";
}

/** @brief Track - Add Audio Input Device
 */
TEST_F(TrackTest, Action2_AddAudioInputDevice)
{
  // Find a valid audio input device
  auto device = DeviceService::instance().get_default_audio_input_device();
  EXPECT_NE(device, nullptr) << "No audio input device found for testing";
  LOG_INFO("Adding audio input device: " + device->to_string());

  // Add audio input to the track
  test_track->add_audio_input(device);
  LOG_INFO("Updated track 0: ", test_track->to_string());

  // Verify the track has an audio input
  EXPECT_TRUE(test_track->has_audio_input());
  EXPECT_EQ(std::get<DeviceHandlePtr>(test_track->get_audio_input()), device);
}

/** @brief Track - Remove Audio Input Device
 */
TEST_F(TrackTest, Action3_RemoveAudioInputDevice)
{
  test_track->add_audio_input(DeviceService::instance().get_default_audio_input_device());

  // Verify the track has an audio input
  EXPECT_TRUE(test_track->has_audio_input()) << "Track should have audio input before removal";

  // Remove audio input
  test_track->remove_audio_input();
  LOG_INFO("Removed audio input from track 0: ", test_track->to_string());

  // Verify the track no longer has an audio input
  EXPECT_FALSE(test_track->has_audio_input()) << "Track should not have audio input after removal";
}

/** @brief Track - Add Audio Input File
 */
TEST_F(TrackTest, Action4_AddAudioInputFile)
{
  // Find a valid audio input device
  auto file = FileService::instance().read_wav_file(TEST_WAV_FILE_PATH);
  EXPECT_TRUE(file.has_value()) << "Failed to read WAV file for testing";
  LOG_INFO("Adding audio input file: ", file.value()->to_string());

  // Add audio input to the track
  test_track->add_audio_input(file.value());
  LOG_INFO("Updated track 0: ", test_track->to_string());

  // Verify the track has an audio input
  EXPECT_TRUE(test_track->has_audio_input());
  EXPECT_EQ(std::get<FileHandlePtr>(test_track->get_audio_input()), file.value());
}

/** @brief Track - Remove Audio Input File
 */
TEST_F(TrackTest, Action5_RemoveAudioInputFile)
{
  test_track->add_audio_input(FileService::instance().read_wav_file(TEST_WAV_FILE_PATH).value());

  // Verify the track has an audio input
  EXPECT_TRUE(test_track->has_audio_input()) << "Track should have audio input before removal";

  // Remove audio input
  test_track->remove_audio_input();
  LOG_INFO("Removed audio input from track 0: ", test_track->to_string());

  // Verify the track no longer has an audio input
  EXPECT_FALSE(test_track->has_audio_input()) << "Track should not have audio input after removal";
}

/** @brief Track - Add Audio Input with Invalid Device
 */
TEST_F(TrackTest, Action6_AddAudioInput_InvalidDevice)
{
  // Get an output device to trigger invalid input
  auto output_device = DeviceService::instance().get_default_audio_output_device();
  try
  {
    test_track->add_audio_input(output_device);
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
  EXPECT_FALSE(test_track->has_audio_input());
}

/** @brief Track - Add MIDI Input Device
 */
TEST_F(TrackTest, Action7_AddMidiInputDevice)
{
  // Find a valid MIDI input device
  auto device = DeviceService::instance().get_default_midi_input_device();
  EXPECT_NE(device, nullptr) << "No MIDI input device found for testing";
  LOG_INFO("Adding MIDI input device: " + device->to_string());

  // Add MIDI input to the track
  test_track->add_midi_input(device);
  LOG_INFO("Updated track 0: ", test_track->to_string());

  // Verify the track has a MIDI input
  EXPECT_TRUE(test_track->has_midi_input());
  EXPECT_EQ(std::get<DeviceHandlePtr>(test_track->get_midi_input()), device);
}

/** @brief Track - Remove MIDI Input Device
 */
TEST_F(TrackTest, Action8_RemoveMidiInputDevice)
{
  test_track->add_midi_input(DeviceService::instance().get_default_midi_input_device());

  // Verify the track has a MIDI input
  EXPECT_TRUE(test_track->has_midi_input()) << "Track should have MIDI input before removal";

  // Remove MIDI input
  test_track->remove_midi_input();
  LOG_INFO("Removed MIDI input from track 0: ", test_track->to_string());

  // Verify the track no longer has a MIDI input
  EXPECT_FALSE(test_track->has_midi_input()) << "Track should not have MIDI input after removal";
}

/** @brief Track - Add MIDI Output
 */
TEST_F(TrackTest, Action9_AddMidiOutput) {
  // Find a valid MIDI output device
  auto device = DeviceService::instance().get_default_midi_output_device();
  EXPECT_NE(device, nullptr) << "No MIDI output device found for testing";
  LOG_INFO("Adding MIDI output device: " + device->to_string());

  // Add MIDI output to the track
  test_track->add_midi_output(device);
  LOG_INFO("Updated track 0: ", test_track->to_string());

  // Verify the track has a MIDI output
  EXPECT_TRUE(test_track->has_midi_output());
  EXPECT_EQ(std::get<DeviceHandlePtr>(test_track->get_midi_output()), device);
}

/** @brief Track - Remove MIDI Output 
 */
TEST_F(TrackTest, Action10_RemoveMidiOutput)
{
  test_track->add_midi_output(DeviceService::instance().get_default_midi_output_device());

  // Verify the track has a MIDI output
  EXPECT_TRUE(test_track->has_midi_output()) << "Track should have MIDI output before removal";

  // Remove MIDI output
  test_track->remove_midi_output();
  LOG_INFO("Removed MIDI output from track 0: ", test_track->to_string());

  // Verify the track no longer has a MIDI output
  EXPECT_FALSE(test_track->has_midi_output()) << "Track should not have MIDI output after removal";
}

TEST_F(TrackTest, Action11_PlayWavFileInput)
{
  // Add audio output
  auto output_device = DeviceService::instance().get_default_audio_output_device();
  EXPECT_NE(output_device, nullptr) << "No audio output device found for testing";

  TrackService::instance().set_audio_output_device(output_device);

  // Open WAV file and add as audio input
  auto file = FileService::instance().read_wav_file(TEST_WAV_FILE_PATH);  
  EXPECT_TRUE(file.has_value()) << "Failed to read WAV file for testing";
  
  test_track->add_audio_input(file.value());
  EXPECT_TRUE(test_track->has_audio_input()) << "Track should have audio input after adding WAV file";
  LOG_INFO("Playing WAV file: ", file.value()->to_string());

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
  EXPECT_NE(stats.audio_output_stats.total_frames_read, 0) << "Total frames processed should be non-negative";
  LOG_INFO("Stopped playing:\n", stats.to_string());
}

TEST_F(TrackTest, Action12_PlayMidiDeviceInput)
{
  // This test requires a physical MIDI input device connected.
  // It will open the device, start playback, and log incoming MIDI messages for 10 seconds.

  // Add MIDI input device
  auto midi_input_device = DeviceService::instance().get_default_midi_input_device();
  EXPECT_NE(midi_input_device, nullptr) << "No MIDI input device found for testing";

  test_track->add_midi_input(midi_input_device);
  EXPECT_TRUE(test_track->has_midi_input()) << "Track should have MIDI input after adding device";
  LOG_INFO("TEST: Using MIDI input device: ", midi_input_device->to_string());

  // // Set up a callback to log incoming MIDI messages
  // test_track->p_midi_dataplane->set_midi_input_callback([](const std::vector<uint8_t>& message) {
  //   std::string msg_str;
  //   for (auto byte : message)
  //     msg_str += " " + std::to_string(byte);
  //   LOG_INFO("Received MIDI message:", msg_str);
  // });

  // Start playback
  EXPECT_TRUE(test_track->play()) << "Failed to start playback";
  EXPECT_TRUE(test_track->is_playing()) << "Track should be playing after play() is called";

  LOG_INFO("Listening for MIDI messages for 5 seconds...");
  std::this_thread::sleep_for(std::chrono::seconds(5));

  LOG_INFO("Stopping playback after 5 seconds.");
  test_track->stop(); // Blocking call
  EXPECT_FALSE(test_track->is_playing()) << "Track should not be playing after stop() is called";

  // Get statistics
  auto stats = test_track->get_statistics();
  EXPECT_NE(stats.midi_input_stats.total_messages_processed, 0) << "Total messages processed should be non-negative";
  LOG_INFO("Stopped playing:\n", stats.to_string());
}

#include <gtest/gtest.h>
#include <thread>
#include <chrono>

#include "miniaudioengine/deviceservice.h"

// Mocks
#include "audiocontroller_mock.h"
#include "device_mock.h"
#include "devicehandle_factory.h"

using namespace miniaudioengine::core;
using namespace miniaudioengine::test;

class AudioControllerTest : public ::testing::Test
{
public:
  AudioControllerTest() = default;
  ~AudioControllerTest() override = default;

  MockAudioControllerPtr get_audio_controller_mock() const
  {
    return p_audio_controller;
  }

  DeviceHandlePtr get_default_audio_output_device() const
  {
    return p_audio_output_device;
  }

  void SetUp() override
  {
    // Initialize mock audio controller
    p_audio_controller = std::make_shared<MockAudioController>();

    // Get audio output device
    p_audio_output_device = p_audio_controller->get_audio_output_device_mock();
    EXPECT_TRUE(p_audio_output_device != nullptr);
    EXPECT_TRUE(p_audio_output_device->is_output());
  }

  void TearDown() override
  {
    // Stop audio stream if playing
    if (p_audio_controller->get_stream_state() == eStreamState::Playing)
    {
      p_audio_controller->stop();
    }
  }

  void set_default_audio_output_device()
  {
    // Get audio output device
    EXPECT_TRUE(p_audio_output_device != nullptr) << "Expected to find a default audio output device";

    p_audio_controller->set_output_device(p_audio_output_device);
    EXPECT_EQ(p_audio_controller->get_device_handle(), p_audio_output_device);
    auto output_device = p_audio_controller->get_device_handle();
    EXPECT_NE(output_device, nullptr) << "Expected output device to be set";
    EXPECT_EQ(*output_device, *p_audio_output_device) << "Expected output device to match the set device";
  }

private:
  DeviceHandlePtr p_audio_output_device;
  MockAudioControllerPtr p_audio_controller;
};

/** @brief Get Audio Devices
 */
TEST_F(AudioControllerTest, GetAudioDevices)
{
  auto devices = get_audio_controller_mock()->get_audio_devices();
  EXPECT_NE(devices.size(), 0) << "Expected to find at least 1 audio device";
}

/** @brief Audio Callback Context Set
 */
TEST_F(AudioControllerTest, AudioCallbackContextSet)
{
  auto callback_context = get_audio_controller_mock()->get_callback_context();
  EXPECT_NE(callback_context, nullptr) << "Expected AudioCallbackContext to be set";
  EXPECT_EQ(callback_context->active_tracks.size(), 0) << "Expected no active tracks initially";
}

/** @brief Set Audio Output Device
 */
TEST_F(AudioControllerTest, SetAudioOutputDevice)
{
  auto device = get_default_audio_output_device();
  EXPECT_TRUE(device != nullptr) << "Expected to find a default audio output device";
  EXPECT_TRUE(device->is_output()) << "Expected audio device to be an output";
  EXPECT_EQ(device->get_input_channels(), 0u) << "Expected device to have 0 input channels";
  EXPECT_NE(device->get_output_channels(), 0u) << "Expected device to have non-zero output channels";

  // Set audio output device
  set_default_audio_output_device();

  // Verify that the output device was set correctly
  auto output_device = get_audio_controller_mock()->get_device_handle();
  EXPECT_NE(output_device, nullptr) << "Expected output device to be set";
  EXPECT_EQ(*output_device, *device) << "Expected output device to match the set device";
}

/** @brief Set Audio Output Device - Not an Output Device
 */
TEST_F(AudioControllerTest, SetAudioOutputDevice_NotAnOutput)
{
  // Create a fake input-only device
  auto input_only_device = DeviceHandleFactory::make_audio(
    9999, "Input Only Device",
    /*is_default_input=*/false, /*is_default_output=*/false,
    /*output_channels=*/0, /*input_channels=*/2, /*duplex=*/0,
    44100, {44100});

  // Attempt to set the input-only device as the output device
  try
  {
    get_audio_controller_mock()->set_output_device(input_only_device);
    FAIL() << "Expected std::invalid_argument exception";
  }
  catch (const std::invalid_argument& e)
  {
    EXPECT_STREQ(e.what(), "AudioController: Device Input Only Device is not an output device.");
  }
  catch (...)
  {
    FAIL() << "Expected std::invalid_argument exception";
  }
}

/** @brief Start Stream - No Active Tracks
 */
TEST_F(AudioControllerTest, StartStream_NoActiveTracks)
{
  set_default_audio_output_device();

  // Start stream
  bool rc = get_audio_controller_mock()->start();
  EXPECT_FALSE(rc) << "Expected stream start to fail due to no active tracks";
}

/** @brief Start Stream
 */
TEST_F(AudioControllerTest, StartStream)
{
  set_default_audio_output_device();

  // TODO - Add active track to TrackManager mock

  // Start stream
  bool rc = get_audio_controller_mock()->start();
  EXPECT_TRUE(rc) << "Expected stream to start successfully";

  auto state = get_audio_controller_mock()->get_stream_state();
  EXPECT_EQ(state, eStreamState::Playing) << "Expected stream state to be Playing";
}

/** @brief Stop Stream
 */
TEST_F(AudioControllerTest, StopStream)
{
  set_default_audio_output_device();

  // Start stream
  get_audio_controller_mock()->start();
  std::this_thread::sleep_for(std::chrono::seconds(1));

  auto state = get_audio_controller_mock()->get_stream_state();
  EXPECT_EQ(state, eStreamState::Playing);
  EXPECT_TRUE(false) << "Expected to fail";

  // Stop stream
  get_audio_controller_mock()->stop();

  std::this_thread::sleep_for(std::chrono::seconds(1));
  state = get_audio_controller_mock()->get_stream_state();
  EXPECT_EQ(state, eStreamState::Stopped);
  EXPECT_TRUE(false) << "Expected to fail";
}

/** @brief Set Audio Output Device - Stream Open
 */
TEST_F(AudioControllerTest, SetAudioOutputDevice_StreamOpen)
{
  set_default_audio_output_device();

  auto device = get_default_audio_output_device();
  EXPECT_TRUE(device != nullptr) << "Expected to find a default audio output device";

  // Start stream
  get_audio_controller_mock()->start();

  auto state = get_audio_controller_mock()->get_stream_state();
  EXPECT_EQ(state, eStreamState::Playing) << "Expected stream state to be Playing";

  // Setting the output device while playing: base class does not auto-stop,
  // so stream state remains Playing (device is updated for next start).
  get_audio_controller_mock()->set_output_device(device);

  auto new_state = get_audio_controller_mock()->get_stream_state();
  EXPECT_EQ(new_state, eStreamState::Playing) << "Expected stream state to remain Playing after updating output device";
}

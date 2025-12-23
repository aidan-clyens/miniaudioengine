#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "devicemanager.h"
#include "audiostreamcontroller.h"
#include "audiodevice.h"

using namespace MinimalAudioEngine;

/** @brief Set Output Device
 */
TEST(AudioStreamControllerTest, SetOutputDevice)
{
  auto device = DeviceManager::instance().get_default_audio_output_device();
  EXPECT_TRUE(device.has_value());
  EXPECT_TRUE(device->is_output());

  AudioStreamController::instance().set_output_device(device.value());
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(device->input_channels, 0);
  EXPECT_NE(device->output_channels, 0);
}

/** @brief Play
 */
TEST(AudioStreamControllerTest, Play)
{
  AudioStreamController::instance().start_stream();

  std::this_thread::sleep_for(std::chrono::seconds(1));
  auto state = AudioStreamController::instance().get_stream_state();
  EXPECT_EQ(state, eAudioState::Playing);
}

/** @brief Stop
 */
TEST(AudioStreamControllerTest, Stop)
{
  auto state = AudioStreamController::instance().get_stream_state();
  EXPECT_EQ(state, eAudioState::Playing);

  AudioStreamController::instance().stop_stream();

  std::this_thread::sleep_for(std::chrono::seconds(1));
  state = AudioStreamController::instance().get_stream_state();
  EXPECT_EQ(state, eAudioState::Stopped);
}

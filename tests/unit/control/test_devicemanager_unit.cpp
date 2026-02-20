#include <gtest/gtest.h>
#include <iostream>

#include "devicemanager.h"
#include "device.h"
#include "logger.h"

using namespace miniaudioengine::control;

class DeviceManagerTest : public ::testing::Test
{
public:
  void SetUp() override
  {
    // This will run before each test
  }

  void TearDown() override
  {
    // This will run after each test
  }

  void set_audio_device_id(unsigned int id)
  {
    audio_device_id = id;
  }

  unsigned int get_audio_device_id() const
  {
    return audio_device_id;
  }

  DeviceManager &device_manager = DeviceManager::instance();

private:
  unsigned int audio_device_id = 0; // Assuming the first audio device has ID 0
};

TEST_F(DeviceManagerTest, Action1_GetAudioDevices)
{
  auto devices = device_manager.get_audio_devices();
  EXPECT_GE(devices.size(), 0);

  for (const auto &device : devices)
  {
    LOG_INFO(device->id, " - ", device->name);
  }

  for (const auto &device : devices)
  {
    LOG_INFO(device->to_string());
  }

  if (!devices.empty())
  {
    set_audio_device_id(devices[0]->id);
  }
}

TEST_F(DeviceManagerTest, Action2_GetAudioDevice)
{
  auto devices = device_manager.get_audio_devices();
  auto device = device_manager.get_audio_device(get_audio_device_id());

  // Attempt to cast to AudioDevice to access specific properties
  auto audio_device = std::dynamic_pointer_cast<AudioDevice>(device);
  EXPECT_NE(audio_device, nullptr) << "Failed to cast to AudioDevice";

  LOG_INFO(audio_device->id, " - ", audio_device->name);

  EXPECT_EQ(audio_device->id, get_audio_device_id());
  EXPECT_EQ(audio_device->id, devices[0]->id);
  EXPECT_EQ(audio_device->name, devices[0]->name);
  EXPECT_EQ(audio_device->input_channels, devices[0]->input_channels);
  EXPECT_EQ(audio_device->output_channels, devices[0]->output_channels);
  EXPECT_EQ(audio_device->duplex_channels, devices[0]->duplex_channels);
  EXPECT_EQ(audio_device->is_default_input, devices[0]->is_default_input);
  EXPECT_EQ(audio_device->is_default_output, devices[0]->is_default_output);
  EXPECT_EQ(audio_device->sample_rates, devices[0]->sample_rates);
  EXPECT_EQ(audio_device->preferred_sample_rate, devices[0]->preferred_sample_rate);
}

TEST_F(DeviceManagerTest, Action3_GetAudioDeviceInvalid)
{
  EXPECT_ANY_THROW({
    auto device = device_manager.get_audio_device(2);
  });
}

TEST_F(DeviceManagerTest, Action4_GetMidiDevices)
{
  auto devices = device_manager.get_midi_devices();
  EXPECT_GE(devices.size(), 0);

  for (const auto &device : devices)
  {
    LOG_INFO(device->id, " - ", device->name);
  }
}

TEST_F(DeviceManagerTest, Action5_GetMidiDevice)
{
  auto devices = device_manager.get_midi_devices();
  auto device = device_manager.get_midi_device(0);

  LOG_INFO(device->id, " - ", device->name);

  EXPECT_EQ(device->id, devices[0]->id);
  EXPECT_EQ(device->name, devices[0]->name);
}

TEST_F(DeviceManagerTest, Action6_GetMidiDeviceInvalid)
{
  EXPECT_ANY_THROW({
     auto device = device_manager.get_midi_device(2);
  });
}
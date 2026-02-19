#include <gtest/gtest.h>
#include <iostream>

#include "devicemanager.h"
#include "device.h"
#include "logger.h"

using namespace miniaudioengine::control;

static unsigned int audio_device_id = 0;

TEST(DeviceManagerTest, GetAudioDevices)
{
  auto devices = DeviceManager::instance().get_audio_devices();
  EXPECT_GE(devices.size(), 0);

  for (const auto &device : devices)
  {
    LOG_INFO(device->id, " - ", device->name);
  }

  audio_device_id = devices.empty() ? 0 : devices[0]->id;
}

TEST(DeviceManagerTest, GetAudioDevice)
{
  auto devices = DeviceManager::instance().get_audio_devices();
  auto device = DeviceManager::instance().get_audio_device(audio_device_id);

  // Attempt to cast to AudioDevice to access specific properties
  auto audio_device = std::dynamic_pointer_cast<AudioDevice>(device);
  EXPECT_NE(audio_device, nullptr) << "Failed to cast to AudioDevice";

  LOG_INFO(audio_device->id, " - ", audio_device->name);

  EXPECT_EQ(audio_device->id, audio_device_id);
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

TEST(DeviceManagerTest, GetAudioDeviceInvalid)
{
  EXPECT_ANY_THROW({
    auto device = DeviceManager::instance().get_audio_device(2);
  });
}

TEST(DeviceManagerTest, GetMidiDevices)
{
  auto devices = DeviceManager::instance().get_midi_devices();
  EXPECT_GE(devices.size(), 0);

  for (const auto &device : devices)
  {
    LOG_INFO(device->id, " - ", device->name);
  }
}

TEST(DeviceManagerTest, GetMidiDevice)
{
  auto devices = DeviceManager::instance().get_midi_devices();

  auto device = DeviceManager::instance().get_midi_device(0);

  LOG_INFO(device->id, " - ", device->name);

  EXPECT_EQ(device->id, devices[0]->id);
  EXPECT_EQ(device->name, devices[0]->name);
}

TEST(DeviceManagerTest, GetMidiDeviceInvalid)
{
  EXPECT_ANY_THROW({
     auto device = DeviceManager::instance().get_midi_device(2);
  });
}
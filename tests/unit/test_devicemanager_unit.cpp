#include <gtest/gtest.h>
#include <iostream>

#include "devicemanager.h"
#include "logger.h"

using namespace miniaudioengine::control;

static unsigned int audio_device_id = 0;

TEST(DeviceManagerTest, GetAudioDevices)
{
  std::vector<AudioDevice> devices = DeviceManager::instance().get_audio_devices();
  EXPECT_GE(devices.size(), 0);

  for (const auto &device : devices)
  {
    LOG_INFO(device.id, " - ", device.name);
  }

  audio_device_id = devices.empty() ? 0 : devices[0].id;
}

TEST(DeviceManagerTest, GetAudioDevice)
{
  std::vector<AudioDevice> devices = DeviceManager::instance().get_audio_devices();

  AudioDevice device = DeviceManager::instance().get_audio_device(audio_device_id);

  LOG_INFO(device.id, " - ", device.name);

  EXPECT_EQ(device.id, audio_device_id);
  EXPECT_EQ(device.id, devices[0].id);
  EXPECT_EQ(device.name, devices[0].name);
  EXPECT_EQ(device.input_channels, devices[0].input_channels);
  EXPECT_EQ(device.output_channels, devices[0].output_channels);
  EXPECT_EQ(device.duplex_channels, devices[0].duplex_channels);
  EXPECT_EQ(device.is_default_input, devices[0].is_default_input);
  EXPECT_EQ(device.is_default_output, devices[0].is_default_output);
  EXPECT_EQ(device.sample_rates, devices[0].sample_rates);
  EXPECT_EQ(device.preferred_sample_rate, devices[0].preferred_sample_rate);
}

TEST(DeviceManagerTest, GetAudioDeviceInvalid)
{
  EXPECT_ANY_THROW({
    AudioDevice device = DeviceManager::instance().get_audio_device(2);
  });
}

TEST(DeviceManagerTest, GetMidiDevices)
{
  std::vector<MidiDevice> devices = DeviceManager::instance().get_midi_devices();
  EXPECT_GE(devices.size(), 0);

  for (const auto &device : devices)
  {
    LOG_INFO(device.id, " - ", device.name);
  }
}

TEST(DeviceManagerTest, GetMidiDevice)
{
  std::vector<MidiDevice> devices = DeviceManager::instance().get_midi_devices();

  MidiDevice device = DeviceManager::instance().get_midi_device(0);

  LOG_INFO(device.id, " - ", device.name);

  EXPECT_EQ(device.id, devices[0].id);
  EXPECT_EQ(device.name, devices[0].name);
}

TEST(DeviceManagerTest, GetMidiDeviceInvalid)
{
  EXPECT_ANY_THROW({
    MidiDevice device = DeviceManager::instance().get_midi_device(2);
  });
}
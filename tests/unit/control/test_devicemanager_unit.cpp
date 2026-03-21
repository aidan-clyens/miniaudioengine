#include <gtest/gtest.h>
#include <iostream>

#include "miniaudioengine/devicemanager.h"
#include "device.h"
#include "logger.h"
#include "audiocontroller_mock.h"
#include "device_mock.h"

using namespace miniaudioengine;
using namespace miniaudioengine::audio;
using namespace miniaudioengine::test;

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

  DeviceManager &device_manager = DeviceManager::instance();
};

TEST_F(DeviceManagerTest, Action1_GetAudioDevices)
{
  auto devices = device_manager.get_audio_devices();
  EXPECT_GT(devices.size(), 0);

  for (const auto &device : devices)
  {
    LOG_INFO(device->id, " - ", device->name);
  }

  for (const auto &device : devices)
  {
    LOG_INFO(device->to_string());
  }
}

TEST_F(DeviceManagerTest, Action2_GetAudioDevice)
{
  auto devices = device_manager.get_audio_devices();
  auto device_1 = devices[0];

  auto device_2 = device_manager.get_audio_device(device_1->id);

  // Attempt to cast to AudioDevice to access specific properties
  auto audio_device = std::dynamic_pointer_cast<AudioDevice>(device_2);
  EXPECT_NE(audio_device, nullptr) << "Failed to cast to AudioDevice";

  LOG_INFO(audio_device->id, " - ", audio_device->name);

  EXPECT_EQ(audio_device->id, device_1->id);
  EXPECT_EQ(audio_device->name, device_1->name);
  EXPECT_EQ(audio_device->input_channels, device_1->input_channels);
  EXPECT_EQ(audio_device->output_channels, device_1->output_channels);
  EXPECT_EQ(audio_device->duplex_channels, device_1->duplex_channels);
  EXPECT_EQ(audio_device->is_default_input, device_1->is_default_input);
  EXPECT_EQ(audio_device->is_default_output, device_1->is_default_output);
  EXPECT_EQ(audio_device->sample_rates, device_1->sample_rates);
  EXPECT_EQ(audio_device->preferred_sample_rate, device_1->preferred_sample_rate);
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
  EXPECT_GT(devices.size(), 0);

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

// ---------------------------------------------------------------------------
// Mock-based unit tests (no real audio/MIDI hardware required)
// ---------------------------------------------------------------------------

class DeviceManagerMockTest : public ::testing::Test
{
protected:
  DeviceManager &device_manager = DeviceManager::instance();

  void SetUp() override
  {
    device_manager.set_audio_controller(std::make_shared<MockAudioController>());
  }

  void TearDown() override
  {
    device_manager.set_audio_controller(std::make_shared<AudioController>());
  }
};

TEST_F(DeviceManagerMockTest, GetAudioDevices_ReturnsTwoMockDevices)
{
  auto devices = device_manager.get_audio_devices();
  EXPECT_EQ(devices.size(), 2u);
}

TEST_F(DeviceManagerMockTest, GetAudioDevices_OutputDeviceProperties)
{
  auto devices = device_manager.get_audio_devices();
  auto output = std::dynamic_pointer_cast<MockAudioOutputDevice>(devices[0]);
  ASSERT_NE(output, nullptr);
  EXPECT_EQ(output->id, 0u);
  EXPECT_EQ(output->name, "Mock Output Device");
  EXPECT_TRUE(output->is_default_output);
  EXPECT_FALSE(output->is_default_input);
  EXPECT_EQ(output->output_channels, 2u);
  EXPECT_EQ(output->input_channels, 0u);
}

TEST_F(DeviceManagerMockTest, GetAudioDevices_InputDeviceProperties)
{
  auto devices = device_manager.get_audio_devices();
  auto input = std::dynamic_pointer_cast<MockAudioOutputDevice>(devices[1]);
  ASSERT_NE(input, nullptr);
  EXPECT_EQ(input->id, 1u);
  EXPECT_EQ(input->name, "Mock Input Device");
  EXPECT_FALSE(input->is_default_output);
  EXPECT_TRUE(input->is_default_input);
  EXPECT_EQ(input->output_channels, 0u);
  EXPECT_EQ(input->input_channels, 2u);
}

TEST_F(DeviceManagerMockTest, GetAudioDevice_ByIdReturnsCorrectDevice)
{
  auto device = device_manager.get_audio_device(1);
  ASSERT_NE(device, nullptr);
  EXPECT_EQ(device->id, 1u);
  EXPECT_EQ(device->name, "Mock Input Device");
}

TEST_F(DeviceManagerMockTest, GetAudioDevice_InvalidIdThrows)
{
  EXPECT_THROW(device_manager.get_audio_device(99), std::out_of_range);
}

TEST_F(DeviceManagerMockTest, GetDefaultAudioOutputDevice_ReturnsMockOutput)
{
  auto device = device_manager.get_default_audio_output_device();
  ASSERT_NE(device, nullptr);
  EXPECT_EQ(device->id, 0u);
  EXPECT_TRUE(device->is_default_output);
}

TEST_F(DeviceManagerMockTest, GetDefaultAudioInputDevice_ReturnsMockInput)
{
  auto device = device_manager.get_default_audio_input_device();
  ASSERT_NE(device, nullptr);
  EXPECT_EQ(device->id, 1u);
  EXPECT_TRUE(device->is_default_input);
}
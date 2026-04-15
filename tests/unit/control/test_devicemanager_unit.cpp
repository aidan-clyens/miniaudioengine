#include <gtest/gtest.h>
#include <iostream>

#include "miniaudioengine/deviceservice.h"
#include "logger.h"
#include "audiocontroller_mock.h"

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

  DeviceService &device_manager = DeviceService::instance();
};

TEST_F(DeviceManagerTest, Action1_GetAudioDevices)
{
  auto devices = device_manager.get_audio_devices();
  EXPECT_GT(devices.size(), 0);

  for (const auto &device : devices)
  {
    LOG_INFO(device->get_id(), " - ", device->get_name());
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

  auto device_2 = device_manager.get_audio_device(device_1->get_id());
  ASSERT_NE(device_2, nullptr) << "get_audio_device should return a valid handle";

  LOG_INFO(device_2->get_id(), " - ", device_2->get_name());

  EXPECT_EQ(device_2->get_id(), device_1->get_id());
  EXPECT_EQ(device_2->get_name(), device_1->get_name());
  EXPECT_EQ(device_2->get_input_channels(), device_1->get_input_channels());
  EXPECT_EQ(device_2->get_output_channels(), device_1->get_output_channels());
  EXPECT_EQ(device_2->get_duplex_channels(), device_1->get_duplex_channels());
  EXPECT_EQ(device_2->is_default_input(), device_1->is_default_input());
  EXPECT_EQ(device_2->is_default_output(), device_1->is_default_output());
  EXPECT_EQ(device_2->get_sample_rates(), device_1->get_sample_rates());
  EXPECT_EQ(device_2->get_preferred_sample_rate(), device_1->get_preferred_sample_rate());
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
    LOG_INFO(device->get_id(), " - ", device->get_name());
  }
}

TEST_F(DeviceManagerTest, Action5_GetMidiDevice)
{
  auto devices = device_manager.get_midi_devices();
  auto device = device_manager.get_midi_device(0);

  LOG_INFO(device->get_id(), " - ", device->get_name());

  EXPECT_EQ(device->get_id(), devices[0]->get_id());
  EXPECT_EQ(device->get_name(), devices[0]->get_name());
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
  DeviceService &device_manager = DeviceService::instance();

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
  auto output = devices[0];
  ASSERT_NE(output, nullptr);
  EXPECT_EQ(output->get_id(), 0u);
  EXPECT_EQ(output->get_name(), "Mock Output Device");
  EXPECT_TRUE(output->is_default_output());
  EXPECT_FALSE(output->is_default_input());
  EXPECT_EQ(output->get_output_channels(), 2u);
  EXPECT_EQ(output->get_input_channels(), 0u);
}

TEST_F(DeviceManagerMockTest, GetAudioDevices_InputDeviceProperties)
{
  auto devices = device_manager.get_audio_devices();
  auto input = devices[1];
  ASSERT_NE(input, nullptr);
  EXPECT_EQ(input->get_id(), 1u);
  EXPECT_EQ(input->get_name(), "Mock Input Device");
  EXPECT_FALSE(input->is_default_output());
  EXPECT_TRUE(input->is_default_input());
  EXPECT_EQ(input->get_output_channels(), 0u);
  EXPECT_EQ(input->get_input_channels(), 2u);
}

TEST_F(DeviceManagerMockTest, GetAudioDevice_ByIdReturnsCorrectDevice)
{
  auto device = device_manager.get_audio_device(1);
  ASSERT_NE(device, nullptr);
  EXPECT_EQ(device->get_id(), 1u);
  EXPECT_EQ(device->get_name(), "Mock Input Device");
}

TEST_F(DeviceManagerMockTest, GetAudioDevice_InvalidIdThrows)
{
  EXPECT_THROW(device_manager.get_audio_device(99), std::out_of_range);
}

TEST_F(DeviceManagerMockTest, GetDefaultAudioOutputDevice_ReturnsMockOutput)
{
  auto device = device_manager.get_default_audio_output_device();
  ASSERT_NE(device, nullptr);
  EXPECT_EQ(device->get_id(), 0u);
  EXPECT_TRUE(device->is_default_output());
}

TEST_F(DeviceManagerMockTest, GetDefaultAudioInputDevice_ReturnsMockInput)
{
  auto device = device_manager.get_default_audio_input_device();
  ASSERT_NE(device, nullptr);
  EXPECT_EQ(device->get_id(), 1u);
  EXPECT_TRUE(device->is_default_input());
}
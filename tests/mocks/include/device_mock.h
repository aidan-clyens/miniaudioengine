#ifndef __DEVICE_MOCK_H__
#define __DEVICE_MOCK_H__

#include <memory>
#include <string>
#include <vector>

// Framework device interface (still used by IController framework-level tests)
#include "device.h"
// DeviceHandle PImpl header and factory (used by IAudioController audio-level tests)
#include "miniaudioengine/devicehandle.h"
#include "devicehandle_factory.h"

using namespace miniaudioengine::core;

namespace miniaudioengine::test
{

// ---------------------------------------------------------------------------
// MockDevice: IAudioDevice implementation for IController (framework) tests
// ---------------------------------------------------------------------------

/** @class MockDevice
 *  @brief A mock implementation of IAudioDevice for framework-level controller tests.
 */
class MockDevice : public IAudioDevice
{
public:
  explicit MockDevice(bool is_input_flag = true, bool is_output_flag = false)
  {
    id = 1;
    name = "Mock Device";
    is_default_output = is_output_flag;
    is_default_input  = is_input_flag;
    output_channels   = is_output_flag ? 2 : 0;
    input_channels    = is_input_flag  ? 2 : 0;
    duplex_channels   = 0;
    preferred_sample_rate = 44100;
  }
};

using MockDevicePtr = std::shared_ptr<MockDevice>;

// ---------------------------------------------------------------------------
// DeviceHandle helpers for IAudioController (audio-level) tests
// ---------------------------------------------------------------------------

// Convenience alias: audio tests use DeviceHandlePtr directly
using MockAudioOutputDevicePtr = DeviceHandlePtr;

/** @brief Creates a mock audio output DeviceHandle for use in tests. */
inline DeviceHandlePtr make_mock_audio_output_device(
  unsigned int id = 0,
  const std::string& name = "Mock Output Device",
  unsigned int output_channels = 2,
  unsigned int input_channels = 0,
  unsigned int preferred_sample_rate = 44100)
{
  return DeviceHandleFactory::make_audio(
    id, name,
    /*is_default_input=*/false, /*is_default_output=*/true,
    output_channels, input_channels, /*duplex=*/0,
    preferred_sample_rate, {44100, 48000});
}

/** @brief Creates a mock audio input DeviceHandle for use in tests. */
inline DeviceHandlePtr make_mock_audio_input_device(
  unsigned int id = 1,
  const std::string& name = "Mock Input Device",
  unsigned int output_channels = 0,
  unsigned int input_channels = 2,
  unsigned int preferred_sample_rate = 44100)
{
  return DeviceHandleFactory::make_audio(
    id, name,
    /*is_default_input=*/true, /*is_default_output=*/false,
    output_channels, input_channels, /*duplex=*/0,
    preferred_sample_rate, {44100, 48000});
}

} // namespace miniaudioengine::test

#endif // __DEVICE_MOCK_H__
#ifndef __DEVICE_MOCK_H__
#define __DEVICE_MOCK_H__

#include <memory>

#include "device.h"

using namespace miniaudioengine::core;

namespace miniaudioengine::test
{

/** @class MockDevice
 *  @brief A mock implementation of the IDevice interface for testing purposes.
 *  This class simulates an audio or MIDI device and can be configured to be an input, output, or both.
 */
class MockDevice : public IAudioDevice
{
public:
  MockDevice(bool is_input = true, bool is_output = false)
  {
    id = 1;
    name = "Mock Device";
    is_default_output = is_output;
    is_default_input = is_input;
  }

  bool is_input() const
  {
    return is_default_input;
  }

  bool is_output() const
  {
    return is_default_output;
  }
};

using MockDevicePtr = std::shared_ptr<MockDevice>;

/** @class MockAudioOutputDevice
 *  @brief A mock output device that simulates an audio output device.
 */
class MockAudioOutputDevice : public MockDevice
{
public:
  MockAudioOutputDevice() : MockDevice(false, true) {}
};

using MockAudioOutputDevicePtr = std::shared_ptr<MockAudioOutputDevice>;

} // namespace miniaudioengine::test

#endif // __DEVICE_MOCK_H__
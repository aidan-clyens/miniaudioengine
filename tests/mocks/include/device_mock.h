#ifndef __DEVICE_MOCK_H__
#define __DEVICE_MOCK_H__

#include <memory>

#include "device.h"

using namespace miniaudioengine::core;

namespace miniaudioengine::test
{

class MockDevice : public IDevice
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

typedef std::shared_ptr<MockDevice> MockDevicePtr;

} // namespace miniaudioengine::test

#endif // __DEVICE_MOCK_H__
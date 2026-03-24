#ifndef __CONTROLLER_MOCK_H__
#define __CONTROLLER_MOCK_H__

#include <memory>

#include "interfaces/controller.h"

using namespace miniaudioengine::core;

namespace miniaudioengine::test
{

class MockController : public IController
{
public:
  MockController() : IController("MockController") {}

private:
  bool _start() override
  {
    return true;
  }

  bool _stop() override
  {
    return true;
  }
};

typedef std::shared_ptr<MockController> MockControllerPtr;

} // namespace miniaudioengine::test

#endif // __CONTROLLER_MOCK_H__
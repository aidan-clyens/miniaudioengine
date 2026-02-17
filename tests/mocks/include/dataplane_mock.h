#ifndef __DATAPLANE_MOCK_H__
#define __DATAPLANE_MOCK_H__

#include <memory>

#include "dataplane.h"

using namespace miniaudioengine::core;

namespace miniaudioengine::test
{

class MockDataPlane : public IDataPlane
{
public:
  bool start() override
  {
    LOG_INFO("MockDataPlane: start() called.");
    return true;
  }

  bool stop() override
  {
    LOG_INFO("MockDataPlane: stop() called.");
    return true;
  }
};

typedef std::shared_ptr<MockDataPlane> MockDataPlanePtr;

} // namespace miniaudioengine::test

#endif // __DATAPLANE_MOCK_H__
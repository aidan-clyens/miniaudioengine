#ifndef __DATAPLANE_MOCK_H__
#define __DATAPLANE_MOCK_H__

#include <memory>

#include "interfaces/dataplane.h"

using namespace miniaudioengine::framework;

namespace miniaudioengine::test
{

/** @brief Mock implementation of IDataPlaneStatistics for unit testing. 
 */
class MockDataStatistics : public IDataPlaneStatistics
{
public:
  size_t frame_counter{0};

  MockDataStatistics() = default;
  ~MockDataStatistics() override = default;

  void reset() override
  {
    frame_counter = 0;
  }
};

using MockDataStatisticsPtr = std::shared_ptr<MockDataStatistics>;

/** @brief Mock implementation of IDataPlane for unit testing. 
 */
class MockDataPlane : public IDataPlane
{
public:
  MockDataPlane(): IDataPlane()
  {
    m_name = "MockDataPlane";
    p_statistics = std::make_shared<MockDataStatistics>();
  }

  ~MockDataPlane() override = default;

private:
  /** @brief Mock implementation of IDataPlane::_start for unit testing.
   */
  bool _start() override
  {
    LOG_INFO("MockDataPlane: start() called.");
    return true;
  }

  /** @brief Mock implementation of IDataPlane::_stop for unit testing.
   */
  bool _stop() override
  {
    LOG_INFO("MockDataPlane: stop() called.");
    return true;
  }
};

typedef std::shared_ptr<MockDataPlane> MockDataPlanePtr;

} // namespace miniaudioengine::test

#endif // __DATAPLANE_MOCK_H__
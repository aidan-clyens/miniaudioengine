#ifndef __DATAPLANE_MOCK_H__
#define __DATAPLANE_MOCK_H__

#include <memory>

#include "dataplane.h"

using namespace miniaudioengine::core;

namespace miniaudioengine::test
{

class MockDataPlane : public IDataPlane
{

};

typedef std::shared_ptr<MockDataPlane> MockDataPlanePtr;

} // namespace miniaudioengine::test

#endif // __DATAPLANE_MOCK_H__
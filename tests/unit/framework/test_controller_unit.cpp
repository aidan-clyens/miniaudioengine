#include <gtest/gtest.h>

#include "controller_mock.h"
#include "dataplane_mock.h"
#include "device_mock.h"

using namespace miniaudioengine::test;

/** @brief Unit tests for the IController class. Uses a mock implementation of the IController interface.
 */
class IControllerUnitTest : public ::testing::Test
{
public:
  MockControllerPtr controller() const
  {
    return p_controller;
  }

  MockDataPlanePtr data_plane() const
  {
    return p_data_plane;
  }

  MockDevicePtr input_device() const
  {
    return p_input_device;
  }

  MockDevicePtr output_device() const
  {
    return p_output_device;
  }

  void SetUp() override
  {
    p_controller = std::make_shared<MockController>();
    p_data_plane = std::make_shared<MockDataPlane>();
    p_input_device = std::make_shared<MockDevice>(true, false);
    p_output_device = std::make_shared<MockDevice>(false, true);
  }

  void TearDown() override
  {
    p_controller.reset();
    p_data_plane.reset();
    p_input_device.reset();
    p_output_device.reset();
  }

private:
  MockControllerPtr p_controller;
  MockDataPlanePtr p_data_plane;
  MockDevicePtr p_input_device;
  MockDevicePtr p_output_device;
};

TEST_F(IControllerUnitTest, RegisterDataPlane)
{
  // Initially, there should be no registered data planes
  auto registered_data_planes = controller()->get_registered_dataplanes();
  EXPECT_EQ(registered_data_planes.size(), 0);

  // Register a data plane and verify it is registered correctly
  controller()->register_dataplane(data_plane());
  registered_data_planes = controller()->get_registered_dataplanes();
  EXPECT_EQ(registered_data_planes.size(), 1);
  EXPECT_EQ(registered_data_planes[0], data_plane());
}

TEST_F(IControllerUnitTest, ClearRegisteredDataPlanes)
{
  // Initially, there should be no registered data planes
  auto registered_data_planes = controller()->get_registered_dataplanes();
  EXPECT_EQ(registered_data_planes.size(), 0);

  // Clear registered data planes when there are none registered
  controller()->clear_registered_dataplane();
  registered_data_planes = controller()->get_registered_dataplanes();
  EXPECT_EQ(registered_data_planes.size(), 0);

  // Register a data plane and then clear it
  controller()->register_dataplane(data_plane());
  registered_data_planes = controller()->get_registered_dataplanes();
  EXPECT_EQ(registered_data_planes.size(), 1);

  controller()->clear_registered_dataplane();
  registered_data_planes = controller()->get_registered_dataplanes();
  EXPECT_EQ(registered_data_planes.size(), 0);
}

TEST_F(IControllerUnitTest, SetOutputDevice)
{
  // Initially, there should be no output device set
  EXPECT_EQ(controller()->get_output_device(), nullptr);

  // Set an output device and verify it is set correctly
  controller()->set_output_device(output_device());
  EXPECT_EQ(controller()->get_output_device(), output_device());

  // Set a null device and verify it throws an exception
  try
  {
    controller()->set_output_device(nullptr);
  }
  catch (const std::invalid_argument &e)
  {
    EXPECT_STREQ(e.what(), "IController: Output device cannot be null.");
  }

  // Set a device that is not an output device
  try
  {
    controller()->set_output_device(input_device());
  }
  catch (const std::invalid_argument &e)
  {
    EXPECT_STREQ(e.what(), ("IController: Device " + input_device()->name + " is not an output device.").c_str());
  }
  EXPECT_EQ(controller()->get_output_device(), output_device());

  // Set a new output device and verify it is updated correctly
  auto new_output_device = std::make_shared<MockDevice>(false, true);
  controller()->set_output_device(new_output_device);
  EXPECT_EQ(controller()->get_output_device(), new_output_device);
}

TEST_F(IControllerUnitTest, Start)
{
  // Verify that the initial stream state is Stopped
  EXPECT_EQ(controller()->get_stream_state(), eStreamState::Stopped);

  // Start the controller when it is stopped
  EXPECT_TRUE(controller()->start());

  // Verify that the stream state is updated to Playing
  EXPECT_EQ(controller()->get_stream_state(), eStreamState::Playing);

  // TODO - Verify that all registered data planes are started

  // Attempt to start the controller again when it is already playing
  // Expected to return false and not change state.
  EXPECT_FALSE(controller()->start());
  EXPECT_EQ(controller()->get_stream_state(), eStreamState::Playing);
}

TEST_F(IControllerUnitTest, Stop)
{
  // Verify that the initial stream state is Stopped
  EXPECT_EQ(controller()->get_stream_state(), eStreamState::Stopped);

  // Stop the controller when it is stopped
  // Expected to return false and not change state.
  EXPECT_FALSE(controller()->stop());

  // Verify that the stream state is updated to Stopped
  EXPECT_EQ(controller()->get_stream_state(), eStreamState::Stopped);
  // TODO - Verify that all registered data planes are stopped

  // Start the controller again and verify it can be restarted after stopping
  EXPECT_TRUE(controller()->start());
  EXPECT_EQ(controller()->get_stream_state(), eStreamState::Playing);

  EXPECT_TRUE(controller()->stop());
  EXPECT_EQ(controller()->get_stream_state(), eStreamState::Stopped);
}
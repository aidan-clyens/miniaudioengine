#include <gtest/gtest.h>
#include <thread>

#include "dataplane_mock.h"

using namespace miniaudioengine::test;

class IDataplaneUnitTest : public ::testing::Test
{
public:
  MockDataPlanePtr dataplane() const { return p_dataplane; }

  void SetUp() override
  {
    p_dataplane = std::make_shared<MockDataPlane>();
  }

  void TearDown() override
  {
    p_dataplane.reset();
  }

private:
  MockDataPlanePtr p_dataplane;
};

TEST_F(IDataplaneUnitTest, Action1_SetInputChannels)
{
  for (size_t i = 0; i < 10; ++i)
  {
    unsigned int channels = static_cast<unsigned int>(i);
    dataplane()->set_input_channels(channels);
    EXPECT_EQ(dataplane()->get_input_channels(), channels);
  }
}

TEST_F(IDataplaneUnitTest, Action2_SetOutputChannels)
{
  for (size_t i = 0; i < 10; ++i)
  {
    unsigned int channels = static_cast<unsigned int>(i);
    dataplane()->set_output_channels(channels);
    EXPECT_EQ(dataplane()->get_output_channels(), channels);
  }
}

TEST_F(IDataplaneUnitTest, Action3_Start)
{
  // Check running state
  EXPECT_FALSE(dataplane()->is_running());

  // Request start and check state
  dataplane()->request_start();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_TRUE(dataplane()->is_running());
}

TEST_F(IDataplaneUnitTest, Action4_Stop)
{
  // Check running state
  EXPECT_FALSE(dataplane()->is_running());

  // Request start and check state
  dataplane()->request_start();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_TRUE(dataplane()->is_running());

  // Request stop and check state
  dataplane()->request_stop();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_FALSE(dataplane()->is_running());
}
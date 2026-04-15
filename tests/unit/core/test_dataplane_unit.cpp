#include <gtest/gtest.h>
#include <thread>

#include "dataplane_mock.h"

using namespace miniaudioengine::test;

class IDataPlaneUnitTest : public ::testing::Test
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

TEST_F(IDataPlaneUnitTest, Action1_SetInputChannels)
{
  for (size_t i = 0; i < 10; ++i)
  {
    unsigned int channels = static_cast<unsigned int>(i);
    dataplane()->set_input_channels(channels);
    EXPECT_EQ(dataplane()->get_input_channels(), channels);
  }
}

TEST_F(IDataPlaneUnitTest, Action2_SetOutputChannels)
{
  for (size_t i = 0; i < 10; ++i)
  {
    unsigned int channels = static_cast<unsigned int>(i);
    dataplane()->set_output_channels(channels);
    EXPECT_EQ(dataplane()->get_output_channels(), channels);
  }
}

TEST_F(IDataPlaneUnitTest, Action3_Start)
{
  // Check running state
  EXPECT_FALSE(dataplane()->is_running());

  // Request start and check state
  dataplane()->start();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_TRUE(dataplane()->is_running());
}

TEST_F(IDataPlaneUnitTest, Action4_Stop)
{
  // Check running state
  EXPECT_FALSE(dataplane()->is_running());

  // Request start and check state
  dataplane()->start();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_TRUE(dataplane()->is_running());

  // Request stop and check state
  dataplane()->stop();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_FALSE(dataplane()->is_running());
}

TEST_F(IDataPlaneUnitTest, Action5_GetStatistics)
{
  auto stats = dataplane()->get_statistics();
  ASSERT_NE(stats, nullptr);

  // Cast to derived class MockDataStatistics
  auto mock_stats = std::dynamic_pointer_cast<MockDataStatistics>(stats);
  ASSERT_NE(mock_stats, nullptr);
  EXPECT_EQ(mock_stats->frame_counter, 0);

  // Modify statistics and check if it reflects in the data plane
  mock_stats->frame_counter = 42;

  auto mock_status_2 = std::dynamic_pointer_cast<MockDataStatistics>(dataplane()->get_statistics());
  ASSERT_NE(mock_status_2, nullptr);

  EXPECT_EQ(mock_status_2->frame_counter, 42);
}

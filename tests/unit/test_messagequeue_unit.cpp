#include <gtest/gtest.h>
#include <iostream>
#include <thread> // for std::jthread
#include <vector> // for std::vector

#include "messagequeue.h" // for MessageQueue
#include "logger.h"

using namespace miniaudioengine::core;

/** @class MessageQueueTest
 *  @brief MessageQueueTest fixture for MessageQueue unit tests. 
 */
class MessageQueueTest : public ::testing::Test
{
public:
  struct ComplexData
  {
    int id;
    std::string name;
    std::vector<float> values;

    bool operator==(const ComplexData& other) const
    {
      return id == other.id && name == other.name && values == other.values;
    }
  };

protected:
  void SetUp() override
  {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  void TearDown() override
  {
    m_queue.stop(); // Ensure queue is stopped at the end of the test
    m_queue.clear();
  }

  /** @brief Get reference to the MessageQueue object.
   *  @return MessageQueue<int>& A reference to the MessageQueue object. 
   */
  MessageQueue<int> &queue()
  {
    return m_queue;
  }

  /** @brief Get reference to the ComplexData MessageQueue object.
   *  @return MessageQueue<ComplexData>& A reference to the ComplexData MessageQueue object. 
   */
  MessageQueue<ComplexData> &complex_queue()
  {
    return m_complex_queue;
  }

private:
  MessageQueue<int> m_queue;
  MessageQueue<ComplexData> m_complex_queue;
};

/** @test Push and Pop operations
 *  This test verifies that messages can be pushed to and popped from the MessageQueue correctly.
 *  Steps:
 *   1. Push a single integer onto the queue.
 *   2. Pop the integer from the queue and verify its value.
 *   3. Attempt to pop from an empty queue and verify that it returns std::nullopt.
 *   4. Push multiple integers onto the queue.
 *   5. Pop each integer and verify their values in order.
 */
TEST_F(MessageQueueTest, PushPop)
{
  const size_t ITEM_COUNT = 5;
  const int TEST_VALUE = 42;

  // Test pushing and popping a single item
  queue().push(TEST_VALUE);
  auto item = queue().pop();
  ASSERT_TRUE(item.has_value());
  EXPECT_EQ(item.value(), TEST_VALUE);

  // Test popping from an empty queue (should block, so we use try_pop)
  auto empty_item = queue().try_pop();
  ASSERT_FALSE(empty_item.has_value());

  // Test pushing and popping multiple items
  for (int i = 0; i < ITEM_COUNT; ++i)
  {
    queue().push(i);
  }

  for (int i = 0; i < ITEM_COUNT; ++i)
  {
    auto multi_item = queue().pop();
    ASSERT_TRUE(multi_item.has_value());
    EXPECT_EQ(multi_item.value(), i);
  }
}

/** @test Multi-threaded Push and Pop operations
 *  This test verifies that the MessageQueue works correctly in a multi-threaded scenario.
 *  Steps:
 *   1. Start a producer thread that pushes a series of integers onto the queue.
 *   2. Start a consumer thread that pops integers from the queue and stores them.
 *   3. Verify that all pushed integers are popped in the correct order.
 */
TEST_F(MessageQueueTest, PushPopMultiThreaded)
{
  const size_t ITEM_COUNT = 10;

  // Producer thread pushes multiple integers onto the shared queue
  std::jthread producer([this]()
  {
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
      queue().push(i);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });

  // Consumer thread pops integers from the shared queue into a vector
  std::jthread consumer([this]()
  {
    std::vector<int> consumed_items;

    for (int i = 0; i < ITEM_COUNT; ++i)
    {
      auto item = queue().pop();
      ASSERT_TRUE(item.has_value());
      consumed_items.push_back(item.value());
    }

    // Verify that all items were consumed in order
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
      EXPECT_EQ(consumed_items[i], i);
    }
  });

  // jthreads will join automatically when going out of scope
}

/** @test Pop blocking behaviour
 *  This test verifies that the pop() method blocks when the queue is empty and unblocks when an item is pushed.
 *  Steps:
 *   1. Start a consumer thread that attempts to pop an item from the queue.
 *   2. Wait for a short duration to ensure the consumer is blocked.
 *   3. Push an item onto the queue from the main thread.
 *   4. Verify that the consumer thread successfully pops the item after it is pushed.
 */
TEST_F(MessageQueueTest, PopBlockingBehaviour)
{
  const int TEST_VALUE = 99;

  // Consumer thread to test blocking pop behaviour
  std::jthread consumer([this, TEST_VALUE]()
  {
    auto item = queue().pop();
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item.value(), TEST_VALUE);
  });

  // Wait to ensure the consumer is blocked on pop()
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Push an item onto the queue to unblock the consumer
  queue().push(TEST_VALUE);

  // jthread will join automatically when going out of scope
}

/** @test TryPop non-blocking behaviour
 *  This test verifies that the try_pop() method does not block and returns std::nullopt when the queue is empty.
 *  Steps:
 *   1. Call try_pop() on an empty queue and verify it returns std::nullopt.
 *   2. Create a consumer thread that calls try_pop() and verify it returns std::nullopt.
 *   2. Push an item onto the queue.
 *   3. The consumer threads calls try_pop() again and verify it returns the pushed item.
 */
TEST_F(MessageQueueTest, TryPopNonBlockingBehaviour)
{
  const int TEST_VALUE = 77;

  // First try_pop should return nullopt
  auto item = queue().try_pop();
  ASSERT_FALSE(item.has_value());

  // Consumer thread to test try_pop behaviour
  std::jthread consumer([this, TEST_VALUE]()
  {
    // First try_pop should return nullopt
    auto item1 = queue().try_pop();
    ASSERT_FALSE(item1.has_value());

    // Wait for an item to be pushed
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Second try_pop should return the pushed item
    auto item2 = queue().try_pop();
    ASSERT_TRUE(item2.has_value());
    EXPECT_EQ(item2.value(), TEST_VALUE);
  });

  // Wait to ensure the consumer has called try_pop()
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // Push an item onto the queue
  queue().push(TEST_VALUE);

  // jthread will join automatically when going out of scope
}

/** @test Stop blocked threads
 *  This test verifies that calling stop() on the MessageQueue unblocks any threads blocked on pop().
 *  Steps:
 *   1. Start a consumer thread that attempts to pop an item from the queue.
 *   2. Wait for a short duration to ensure the consumer is blocked.
 *   3. Call stop() on the queue from the main thread.
 *   4. Verify that the consumer thread unblocks and receives std::nullopt.
 */
TEST_F(MessageQueueTest, StopBlockedThreads)
{
  // Consumer thread to test stop behaviour
  std::jthread consumer([this]()
  {
    auto item = queue().pop();
    ASSERT_FALSE(item.has_value()); // Should receive nullopt after stop
  });

  // Wait to ensure the consumer is blocked on pop()
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Call stop to unblock the consumer
  queue().stop();

  // jthread will join automatically when going out of scope
}

/** @test Push and Pop with complex data types 
 *  This test verifies that the MessageQueue can handle complex data types correctly.
 *  Steps:
 *   1. Define a complex data type (e.g., struct).
 *   2. Push an instance of the complex data type onto the queue.
 *   3. Pop the instance from the queue and verify its contents.
 *   4. Repeat with multiple instances to verify order and integrity.
  */
TEST_F(MessageQueueTest, PushPopComplexData)
{
  MessageQueueTest::ComplexData data_to_push{1, "TestName", {0.1f, 0.2f, 0.3f}};
  complex_queue().push(data_to_push);

  auto popped_data = complex_queue().pop();
  ASSERT_TRUE(popped_data.has_value());
  EXPECT_EQ(popped_data.value(), data_to_push);

  // Test pushing and popping multiple complex data items
  const std::vector<MessageQueueTest::ComplexData> expected_data = {
    {2, "Name2", {1.1f, 1.2f}},
    {3, "Name3", {2.1f, 2.2f}},
    {4, "Name4", {3.1f, 3.2f}},
    {5, "Name5", {4.1f, 4.2f}},
    {6, "Name6", {5.1f, 5.2f}}
  };

  // Push all complex data items onto the queue
  for (const auto& data : expected_data)
  {
    complex_queue().push(data);
  }

  // Pop and verify each complex data item
  for (const auto& expected : expected_data)
  {
    auto item = complex_queue().pop();
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item.value(), expected);
  }
}

/** @test Push and Pop complex data types in multi-threaded scenario
 *  This test verifies that the MessageQueue can handle complex data types correctly in a multi-threaded scenario.
 *  Steps:
 *   1. Start a producer thread that pushes multiple instances of a complex data type onto the queue.
 *   2. Start a consumer thread that pops instances from the queue and verifies their contents.
 *   3. Ensure all pushed instances are popped in the correct order.
 */
TEST_F(MessageQueueTest, PushPopComplexDataMulitThreaded)
{
  // Test pushing and popping multiple complex data items
  const std::vector<MessageQueueTest::ComplexData> expected_data = {
      {2, "Name2", {1.1f, 1.2f}},
      {3, "Name3", {2.1f, 2.2f}},
      {4, "Name4", {3.1f, 3.2f}},
      {5, "Name5", {4.1f, 4.2f}},
      {6, "Name6", {5.1f, 5.2f}}};

  // Producer thread pushes complex data items onto the shared queue
  std::jthread producer([this, &expected_data]()
  {
    for (const auto& data : expected_data)
    {
      complex_queue().push(data);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });

  // Consumer thread pops complex data items from the shared queue and verifies them
  std::jthread consumer([this, &expected_data]()
  {
    for (const auto& expected : expected_data)
    {
      auto item = complex_queue().pop();
      ASSERT_TRUE(item.has_value());
      EXPECT_EQ(item.value(), expected);
    }
  });
}
#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

#include "lockfree_ringbuffer.h"

using namespace MinimalAudioEngine;

class LockfreeRingBufferTest : public ::testing::Test
{
public:
  static constexpr size_t BUFFER_SIZE = 1024;
  static constexpr size_t USABLE_BUFFER_SIZE = BUFFER_SIZE - 1; // One slot is reserved to distinguish full vs empty

  void SetUp() override
  {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  void TearDown() override
  {
    m_ring_buffer.clear();
    EXPECT_EQ(m_ring_buffer.size(), 0) << "Ring buffer should be empty after TearDown";
  }

  LockfreeRingBuffer<int, BUFFER_SIZE>& ring_buffer()
  {
    return m_ring_buffer;
  }

private:
  LockfreeRingBuffer<int, BUFFER_SIZE> m_ring_buffer;
};

template<int Capacity>
static void test_performance(int iterations)
{
  EXPECT_NE(iterations, 0) << "Iterations must be greater than zero";

  LockfreeRingBuffer<int, Capacity> ring_buffer;

  std::atomic<int> producer_count{0};
  std::atomic<int> consumer_count{0};
  std::atomic<bool> producer_done{false};

  auto start_time = std::chrono::high_resolution_clock::now();

  std::jthread producer([&]()
                        {
    for (int i = 0; i < iterations; ++i)
    {
      while (!ring_buffer.try_push(i))
      {
        // Busy wait if the buffer is full
      }
      producer_count.fetch_add(1, std::memory_order_relaxed);
    }
    producer_done.store(true, std::memory_order_release); });

  std::jthread consumer([&]()
                        {
    int item;
    for (int i = 0; i < iterations; ++i)
    {
      while (!ring_buffer.try_pop(item))
      {
        // Busy wait if the buffer is empty
        // Ensure we don't deadlock if producer finished early
        if (producer_done.load(std::memory_order_acquire) && ring_buffer.size() == 0)
        {
          FAIL() << "Consumer starved - only received " << i << " items out of " << iterations;
        }
      }
      consumer_count.fetch_add(1, std::memory_order_relaxed);
      EXPECT_EQ(item, i) << "FIFO order violated at iteration " << i;
    } });

  producer.join();
  consumer.join();

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

  // Verify counts
  EXPECT_EQ(producer_count.load(), iterations) << "Producer did not push all items";
  EXPECT_EQ(consumer_count.load(), iterations) << "Consumer did not pop all items";

  // Verify buffer is empty
  EXPECT_EQ(ring_buffer.size(), 0) << "Buffer should be empty after test";

  // Performance reporting (not an assertion, just informational)
  std::cout << "Completed " << iterations << " push/pop operations in "
            << duration.count() << " ms ("
            << (iterations * 1000.0 / duration.count()) << " ops/sec)" << std::endl;
}

/** @test Create LockfreeRingBuffer 
 *  This test verifies that a LockfreeRingBuffer can be created successfully and has the expected initial state.
 *  Steps:
 *   1. Check that the returned capacity of the ring buffer matches the expected usable buffer size.
 *   2. Check that the initial size of the ring buffer is zero.
 */
TEST_F(LockfreeRingBufferTest, CreateRingBuffer)
{
  EXPECT_EQ(ring_buffer().capacity(), LockfreeRingBufferTest::USABLE_BUFFER_SIZE) << "Expected capacity is " << LockfreeRingBufferTest::USABLE_BUFFER_SIZE << " but actual capacity is " << ring_buffer().capacity();
  EXPECT_EQ(ring_buffer().size(), 0) << "Expected size is 0 but actual size is " << ring_buffer().size();
}

TEST_F(LockfreeRingBufferTest, PushPopSingleThread)
{
  EXPECT_TRUE(ring_buffer().try_push(1));
  EXPECT_TRUE(ring_buffer().try_push(2));
  EXPECT_TRUE(ring_buffer().try_push(3));

  EXPECT_EQ(ring_buffer().size(), 3) << "Expected size is 3 but actual size is " << ring_buffer().size();

  int item;
  EXPECT_TRUE(ring_buffer().try_pop(item));
  EXPECT_EQ(item, 1);
  EXPECT_TRUE(ring_buffer().try_pop(item));
  EXPECT_EQ(item, 2);
  EXPECT_TRUE(ring_buffer().try_pop(item));
  EXPECT_EQ(item, 3);

  EXPECT_EQ(ring_buffer().size(), 0) << "Expected size is 0 but actual size is " << ring_buffer().size();
}

TEST_F(LockfreeRingBufferTest, PushOverfill)
{
  const size_t expected_size = ring_buffer().capacity(); // Should be full but not overfilled
  
  // Fill the buffer to capacity
  for (int i = 1; i <= ring_buffer().capacity(); ++i)
  {
    EXPECT_TRUE(ring_buffer().try_push(i));
  }
  EXPECT_EQ(ring_buffer().size(), expected_size) << "Expected size is " << expected_size << " but actual size is " << ring_buffer().size();

  // Attempt to overfill the buffer
  const size_t extra_pushes = 5;

  for (int i = 0; i < extra_pushes; ++i)
  {
    EXPECT_FALSE(ring_buffer().try_push(100 + i)) << "Overfill push should have failed at iteration " << i;
  }
  EXPECT_EQ(ring_buffer().size(), expected_size) << "Expected size is " << expected_size << " but actual size is " << ring_buffer().size();
}

TEST_F(LockfreeRingBufferTest, PushPopPerformance)
{
  const int iterations = 100000;
  test_performance<1024>(iterations);
}

TEST_F(LockfreeRingBufferTest, HighContentionPerformance)
{
  const int iterations = 100000;
  test_performance<8>(iterations);
}

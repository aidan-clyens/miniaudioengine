#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

#include "lockfree_ringbuffer.h"

using namespace MinimalAudioEngine;

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

TEST(RealTime_LockfreeRingBuffer_Test, CreateRingBuffer)
{
  const int capacity = 1024;
  const int usable_capacity = capacity - 1;

  LockfreeRingBuffer<int, capacity> ring_buffer;
  EXPECT_EQ(ring_buffer.capacity(), usable_capacity) << "Expected capacity is " << usable_capacity << " but actual capacity is " << ring_buffer.capacity();
  EXPECT_EQ(ring_buffer.size(), 0) << "Expected size is 0 but actual size is " << ring_buffer.size();
}

TEST(RealTime_LockfreeRingBuffer_Test, PushPopSingleThread)
{
  const int capacity = 4;
  LockfreeRingBuffer<int, capacity> ring_buffer;

  EXPECT_TRUE(ring_buffer.try_push(1));
  EXPECT_TRUE(ring_buffer.try_push(2));
  EXPECT_TRUE(ring_buffer.try_push(3));

  EXPECT_EQ(ring_buffer.size(), 3) << "Expected size is 3 but actual size is " << ring_buffer.size();

  int item;
  EXPECT_TRUE(ring_buffer.try_pop(item));
  EXPECT_EQ(item, 1);
  EXPECT_TRUE(ring_buffer.try_pop(item));
  EXPECT_EQ(item, 2);
  EXPECT_TRUE(ring_buffer.try_pop(item));
  EXPECT_EQ(item, 3);

  EXPECT_EQ(ring_buffer.size(), 0) << "Expected size is 0 but actual size is " << ring_buffer.size();
}

TEST(RealTime_LockfreeRingBuffer_Test, PushPopOverfill)
{
  const int capacity = 4;
  LockfreeRingBuffer<int, capacity> ring_buffer;

  EXPECT_TRUE(ring_buffer.try_push(1));
  EXPECT_TRUE(ring_buffer.try_push(2));
  EXPECT_TRUE(ring_buffer.try_push(3));
  EXPECT_FALSE(ring_buffer.try_push(4)); // This should fail or be ignored

  EXPECT_EQ(ring_buffer.size(), 3) << "Expected size is 3 but actual size is " << ring_buffer.size();

  int item;
  EXPECT_TRUE(ring_buffer.try_pop(item));
  EXPECT_EQ(item, 1);
  EXPECT_TRUE(ring_buffer.try_pop(item));
  EXPECT_EQ(item, 2);
  EXPECT_TRUE(ring_buffer.try_pop(item));
  EXPECT_EQ(item, 3);
  EXPECT_FALSE(ring_buffer.try_pop(item)); // This should fail

  EXPECT_EQ(ring_buffer.size(), 0) << "Expected size is 0 but actual size is " << ring_buffer.size();
}

TEST(RealTime_LockfreeRingBuffer_Test, PushPopWrapAround)
{
  const int capacity = 4;
  LockfreeRingBuffer<int, capacity> ring_buffer;

  EXPECT_TRUE(ring_buffer.try_push(1));
  EXPECT_TRUE(ring_buffer.try_push(2));
  EXPECT_TRUE(ring_buffer.try_push(3));

  int item;
  EXPECT_TRUE(ring_buffer.try_pop(item));
  EXPECT_EQ(item, 1);
  EXPECT_TRUE(ring_buffer.try_pop(item));
  EXPECT_EQ(item, 2);

  EXPECT_TRUE(ring_buffer.try_push(4));
  EXPECT_TRUE(ring_buffer.try_push(5)); // This should wrap around

  EXPECT_EQ(ring_buffer.size(), 3) << "Expected size is 3 but actual size is " << ring_buffer.size();

  EXPECT_TRUE(ring_buffer.try_pop(item));
  EXPECT_EQ(item, 3);
  EXPECT_TRUE(ring_buffer.try_pop(item));
  EXPECT_EQ(item, 4);
  EXPECT_TRUE(ring_buffer.try_pop(item));
  EXPECT_EQ(item, 5);

  EXPECT_EQ(ring_buffer.size(), 0) << "Expected size is 0 but actual size is " << ring_buffer.size();
}

TEST(RealTime_LockfreeRingBuffer_Test, PushPopPerformance)
{
  const int iterations = 100000;
  test_performance<1024>(iterations);
}

TEST(RealTime_LockfreeRingBuffer_Test, HighContentionPerformance)
{
  const int iterations = 100000;
  test_performance<8>(iterations);
}

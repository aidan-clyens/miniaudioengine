#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <numeric>
#include <algorithm>

#include "lockfree_ringbuffer.h"
#include "logger.h"

using namespace miniaudioengine::core;

static std::vector<int> generate_int_sample_data(size_t num_samples)
{
  std::vector<int> data(num_samples);
  for (size_t i = 0; i < num_samples; ++i)
  {
    data[i] = rand() % 100; // Random integers between 0 and 99
  }
  return data;
}

template <typename T, size_t Size>
static LockfreeRingBuffer<T, Size> create_ringbuffer()
{
  return LockfreeRingBuffer<T, Size>();
}

/** @test Profile LockfreeRingBuffer with Single Producer and Single Consumer
 *  This test profiles the performance of the LockfreeRingBuffer under a single producer and single consumer scenario
 *  at different sample rates and buffer sizes.
 *  Steps:
 *  1. Create a LockfreeRingBuffer instance with a buffer size of 1024 and samples rate of 44100 Hz.
 *  2. Create a producer thread that pushes data into the ring buffer at the specified sample rate.
 *  3. Create a consumer thread that pops data from the ring buffer at the specified sample rate.
 *  4. Measure the time taken for push and pop operations over a fixed duration (e.g., 10 seconds).
 *  5. Calculate and log the throughput (items per second) for both push and pop operations.
 *  6. Repeat the profiling for different buffer sizes (e.g., 256, 512, 2048) and sample rates (e.g., 48000 Hz, 96000 Hz).
 *  7. Analyze the results to determine the performance characteristics of the LockfreeRingBuffer under varying conditions.
 */
TEST(LockfreeRingBufferProfilingTest, SingleProducerSingleConsumer)
{
  const size_t buffer_size = 512; // Capacity of the ring buffer
  const unsigned int sample_rate = 44100; // Hz
  const size_t batch_size = buffer_size;  // Samples per batch
  const double test_duration_seconds = 60; // Duration of the test in seconds
  const size_t num_samples = static_cast<size_t>(sample_rate * test_duration_seconds); // Total samples for the test duration
  const double time_per_sample_us = 1000000.0 / sample_rate; // microseconds per sample
  const double time_per_batch_us = time_per_sample_us * batch_size * 0.9; // microseconds per batch (90% of target time to leave room for scheduling delays)

  LOG_DEBUG("Starting LockfreeRingBuffer profiling test with buffer size: ", buffer_size,
            ", sample rate: ", sample_rate, " Hz, batch size: ", batch_size,
            ", test duration: ", test_duration_seconds, " seconds",
            ", time per batch: ", time_per_batch_us, " us.");

  // Create sample data to push into the ring buffer
  std::vector<int> sample_data = generate_int_sample_data(num_samples);
  EXPECT_EQ(sample_data.size(), num_samples) << "Sample data size mismatch. Expected "
                                             << num_samples << " samples. Got " << sample_data.size();

  // Create the ring buffer
  auto ring_buffer = create_ringbuffer<int, buffer_size>();
  
  // Create signal to indicate end of production
  std::atomic<bool> done_producing{false};

  // Create producer thread
  std::jthread producer([&ring_buffer, &sample_data, &done_producing, time_per_batch_us, batch_size]()
  {
    set_thread_name("Producer");

    const size_t num_batches = (sample_data.size() + batch_size - 1) / batch_size;
    LOG_DEBUG("Producing ", sample_data.size(), " total samples in ", num_batches, " batches of ", batch_size, " samples.");
    LOG_DEBUG("Time per batch: ", time_per_batch_us, " us (", time_per_batch_us / 1000.0, " ms). Expected duration: ",
              (num_batches * time_per_batch_us) / 1000000.0, " seconds.");

    std::vector<double> batch_interval_times_us;
    auto start_time = std::chrono::high_resolution_clock::now();
    auto last_batch_time = start_time;
    
    size_t batch_num = 0;
    for (size_t i = 0; i < sample_data.size(); i += batch_size)
    {
      // Calculate when this batch should be pushed
      auto target_time = start_time + std::chrono::duration<double, std::micro>(batch_num * time_per_batch_us);

      // Sleep until the target time
      auto now = std::chrono::high_resolution_clock::now();
      if (target_time > now)
      {
        std::this_thread::sleep_until(target_time);
      }

      auto batch_start = std::chrono::high_resolution_clock::now();

      // Determine actual batch size (last batch might be smaller)
      size_t current_batch_size = std::min(batch_size, sample_data.size() - i);
      
      // Push batch of samples
      LOG_DEBUG("Producing batch ", batch_num, " (samples ", i, " to ", i + current_batch_size - 1, ")");
      
      for (size_t j = 0; j < current_batch_size; ++j)
      {
        // Push the sample (with retry if buffer is full)
        while (!ring_buffer.try_push(sample_data[i + j]))
        {
          // Buffer is full, yield to allow consumer to catch up
          std::this_thread::yield();
        }
      }

      auto batch_end = std::chrono::high_resolution_clock::now();
      
      // Measure actual interval from last batch
      if (batch_num > 0)
      {
        std::chrono::duration<double, std::micro> interval = batch_start - last_batch_time;
        batch_interval_times_us.push_back(interval.count());
      }
      
      last_batch_time = batch_start;
      ++batch_num;
    }

    // Signal end of production
    done_producing.store(true);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    LOG_DEBUG("Producer thread completed producing ", batch_num, " batches in ", elapsed.count(), " seconds.");

    // Calculate batch interval time statistics
    if (!batch_interval_times_us.empty())
    {
      double sum = std::accumulate(batch_interval_times_us.begin(), batch_interval_times_us.end(), 0.0);
      double mean = sum / batch_interval_times_us.size();

      std::vector<double> diff(batch_interval_times_us.size());
      std::transform(batch_interval_times_us.begin(), batch_interval_times_us.end(), diff.begin(),
                     [mean](double x) { return x - mean; });
      double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
      double stdev = std::sqrt(sq_sum / batch_interval_times_us.size());

      // Find min and max intervals
      double min_interval = *std::min_element(batch_interval_times_us.begin(), batch_interval_times_us.end());
      double max_interval = *std::max_element(batch_interval_times_us.begin(), batch_interval_times_us.end());

      LOG_DEBUG("Batch interval time - mean: ", mean, " us, stdev: ", stdev, " us, min: ", min_interval, " us, max: ", max_interval, " us.");
      LOG_DEBUG("Target batch interval was: ", time_per_batch_us, " us.");
    }

    // Calculate actual throughput
    double throughput = static_cast<double>(sample_data.size()) / elapsed.count();
    double target_throughput = 1000000.0 / (time_per_batch_us / batch_size);
    LOG_DEBUG("Producer throughput: ", throughput, " Hz (target: ", target_throughput, " Hz).");

    // Expect the calculated throughput to be within 10% of target sample rate
    EXPECT_NEAR(throughput, target_throughput, target_throughput * 0.1) << "Producer throughput deviates more than 10% from target sample rate.";
  });

  // Add slight delay to ensure producer starts first
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // Create consumer thread
  std::jthread consumer([&ring_buffer, &sample_data, &done_producing, time_per_sample_us]()
  {
    set_thread_name("Consumer");

    std::vector<int> consumed_samples;

    auto start_time = std::chrono::high_resolution_clock::now();
    int sample;
    size_t index = 0;
    while (!done_producing.load() || ring_buffer.size() > 0)
    {
      // If received all samples, break
      if (index >= sample_data.size())
      {
        break;
      }

      if (ring_buffer.try_pop(sample))
      {
        consumed_samples.push_back(sample);

        // Print every batch of samples consumed
        if (index % batch_size == 0)
          LOG_DEBUG("Consuming sample: ", index, " - ", sample);
        ++index;
      }
      else
      {
        // Busy wait for data to be available
        // Buffer is empty, yield to allow producer to add more data
        std::this_thread::yield();
      }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    LOG_DEBUG("Consumer thread completed consuming samples in ", elapsed.count(), " seconds.");

    // Calculate actual throughput
    double throughput = static_cast<double>(index) / elapsed.count();
    LOG_DEBUG("Consumer throughput: ", throughput, " Hz.");

    // Verify data integrity
    EXPECT_EQ(index, consumed_samples.size());
    EXPECT_EQ(index, sample_data.size());

    if (index != consumed_samples.size() || index != sample_data.size())
    {
      LOG_ERROR("Data size mismatch: produced ", sample_data.size(), " samples, consumed ", consumed_samples.size(), " samples.");
      return;
    }

    size_t correct_samples_count = 0;
    for (size_t i = 0; i < sample_data.size(); ++i)
    {
      EXPECT_EQ(sample_data[i], consumed_samples[i]) << "Data mismatch at sample index " << i;
      if (sample_data[i] == consumed_samples[i])
      {
        ++correct_samples_count;
      }
    }

    LOG_DEBUG("Data integrity check: ", correct_samples_count, " out of ", sample_data.size(), " samples matched (",
              (static_cast<double>(correct_samples_count) / sample_data.size()) * 100.0, "%).");
  });

  // Wait for producer and consumer to finish
  producer.join();
  consumer.join();
}
#include <gtest/gtest.h>
#include <vector>
#include <atomic>
#include <thread>

#include "audiobatchproducer.h"
#include "lockfree_ringbuffer.h"
#include "logger.h"

using namespace MinimalAudioEngine;

/** @test Test AudioBatchProducer basic functionality */
TEST(AudioBatchProducerTest, BasicFunctionality)
{
  const size_t buffer_size = 512;
  const size_t num_samples = 1000;
  const unsigned int sample_rate = 44100;
  const size_t batch_size = 100;

  // Create sample data
  std::vector<int> sample_data(num_samples);
  for (size_t i = 0; i < num_samples; ++i)
  {
    sample_data[i] = static_cast<int>(i);
  }

  // Create ring buffer
  LockfreeRingBuffer<int, buffer_size> ring_buffer;

  // Configure producer
  AudioBatchProducerConfig config;
  config.sample_rate = sample_rate;
  config.batch_size = batch_size;

  // Create producer
  AudioBatchProducer<int, buffer_size> producer(ring_buffer, sample_data, config);

  // Set completion callback
  bool callback_invoked = false;
  AudioBatchProducerStats final_stats;
  producer.set_completion_callback([&callback_invoked, &final_stats](const AudioBatchProducerStats& stats) {
    callback_invoked = true;
    final_stats = stats;
    LOG_INFO("Producer completed with stats:\n", stats.to_string());
  });

  // Start producer
  producer.start();
  EXPECT_TRUE(producer.is_running());

  // Consume samples as they arrive
  std::vector<int> consumed_data;
  consumed_data.reserve(num_samples);

  while (!producer.is_done() || ring_buffer.size() > 0)
  {
    int sample;
    if (ring_buffer.try_pop(sample))
    {
      consumed_data.push_back(sample);
    }
    else
    {
      std::this_thread::yield();
    }
  }

  // Wait for producer to finish
  producer.stop();
  EXPECT_FALSE(producer.is_running());

  // Verify callback was invoked
  EXPECT_TRUE(callback_invoked);

  // Verify all samples were produced and consumed
  EXPECT_EQ(consumed_data.size(), num_samples);

  // Verify data integrity
  for (size_t i = 0; i < num_samples; ++i)
  {
    EXPECT_EQ(consumed_data[i], sample_data[i]) << "Data mismatch at index " << i;
  }

  // Verify statistics
  EXPECT_GT(final_stats.total_batches, 0);
  EXPECT_GT(final_stats.throughput_hz, 0);
  EXPECT_NEAR(final_stats.throughput_hz, sample_rate, sample_rate * 0.15); // Within 15% of target
}

/** @test Test AudioBatchProducer with different batch sizes */
TEST(AudioBatchProducerTest, DifferentBatchSizes)
{
  const size_t buffer_size = 1024;
  const size_t num_samples = 5000;
  const unsigned int sample_rate = 44100;

  std::vector<size_t> batch_sizes = {64, 128, 256, 512};

  for (size_t batch_size : batch_sizes)
  {
    LOG_INFO("Testing with batch size: ", batch_size);

    // Create sample data
    std::vector<float> sample_data(num_samples);
    for (size_t i = 0; i < num_samples; ++i)
    {
      sample_data[i] = static_cast<float>(i) * 0.01f;
    }

    // Create ring buffer
    LockfreeRingBuffer<float, buffer_size> ring_buffer;

    // Configure producer
    AudioBatchProducerConfig config;
    config.sample_rate = sample_rate;
    config.batch_size = batch_size;

    // Create and start producer
    AudioBatchProducer<float, buffer_size> producer(ring_buffer, sample_data, config);
    producer.start();

    // Consume samples
    std::vector<float> consumed_data;
    consumed_data.reserve(num_samples);

    while (!producer.is_done() || ring_buffer.size() > 0)
    {
      float sample;
      if (ring_buffer.try_pop(sample))
      {
        consumed_data.push_back(sample);
      }
      else
      {
        std::this_thread::yield();
      }
    }

    producer.stop();

    // Verify
    EXPECT_EQ(consumed_data.size(), num_samples) << "Failed with batch size: " << batch_size;

    auto stats = producer.get_stats();
    LOG_INFO("Batch size ", batch_size, " - Throughput: ", stats.throughput_hz, " Hz");
  }
}

/** @test Test AudioBatchProducer early stop */
TEST(AudioBatchProducerTest, EarlyStop)
{
  const size_t buffer_size = 512;
  const size_t num_samples = 100000; // Large number
  const unsigned int sample_rate = 44100;
  const size_t batch_size = 256;

  // Create sample data
  std::vector<int> sample_data(num_samples);
  for (size_t i = 0; i < num_samples; ++i)
  {
    sample_data[i] = static_cast<int>(i);
  }

  // Create ring buffer
  LockfreeRingBuffer<int, buffer_size> ring_buffer;

  // Configure producer
  AudioBatchProducerConfig config;
  config.sample_rate = sample_rate;
  config.batch_size = batch_size;

  // Create and start producer
  AudioBatchProducer<int, buffer_size> producer(ring_buffer, sample_data, config);
  producer.start();

  // Let it run briefly
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Stop early
  producer.stop();

  EXPECT_FALSE(producer.is_running());
  
  // Verify some samples were produced but not all
  size_t consumed_count = 0;
  int sample;
  while (ring_buffer.try_pop(sample))
  {
    consumed_count++;
  }

  LOG_INFO("Early stop: Consumed ", consumed_count, " samples out of ", num_samples);
  EXPECT_GT(consumed_count, 0);
  EXPECT_LT(consumed_count, num_samples);
}

/** @test Test AudioBatchProducer timing accuracy at 44100 Hz */
TEST(AudioBatchProducerTest, TimingAccuracy44100Hz)
{
  const size_t buffer_size = 1024;
  const size_t num_samples = 10000;
  const unsigned int sample_rate = 44100;
  const size_t batch_size = 512;

  // Create sample data
  std::vector<float> sample_data(num_samples, 1.0f);

  // Create ring buffer
  LockfreeRingBuffer<float, buffer_size> ring_buffer;

  // Configure producer
  AudioBatchProducerConfig config;
  config.sample_rate = sample_rate;
  config.batch_size = batch_size;
  config.batch_time_multiplier = 0.9; // 90% of ideal time

  // Create producer
  AudioBatchProducer<float, buffer_size> producer(ring_buffer, sample_data, config);
  producer.start();

  // Consume samples
  size_t consumed_count = 0;
  while (!producer.is_done() || ring_buffer.size() > 0)
  {
    float sample;
    if (ring_buffer.try_pop(sample))
    {
      consumed_count++;
    }
    else
    {
      std::this_thread::yield();
    }
  }

  producer.stop();

  // Get stats
  auto stats = producer.get_stats();

  // Verify timing accuracy
  EXPECT_EQ(consumed_count, num_samples);
  EXPECT_GE(stats.throughput_hz, sample_rate); // Faster than or equal to target
  EXPECT_NEAR(stats.throughput_hz, sample_rate, sample_rate * 0.15); // Within 15% of target

  LOG_INFO("Timing test at 44100 Hz:");
  LOG_INFO("  Target throughput: ", sample_rate, " Hz");
  LOG_INFO("  Actual throughput: ", stats.throughput_hz, " Hz");
  LOG_INFO("  Mean batch interval: ", stats.mean_interval_us, " us");
  LOG_INFO("  Stdev batch interval: ", stats.stdev_interval_us, " us");
}

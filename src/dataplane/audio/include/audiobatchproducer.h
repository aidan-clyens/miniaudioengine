#ifndef __AUDIO_BATCH_PRODUCER_H__
#define __AUDIO_BATCH_PRODUCER_H__

#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <functional>
#include <sstream>

#include "lockfree_ringbuffer.h"
#include "logger.h"

namespace MinimalAudioEngine
{

/** @struct AudioBatchProducerConfig
 *  @brief Configuration for the AudioBatchProducer
 */
struct AudioBatchProducerConfig
{
  unsigned int sample_rate = 44100;        // Samples per second
  size_t batch_size = 512;                 // Samples per batch
  double batch_time_multiplier = 0.9;      // Multiplier for batch time (to account for scheduling delays)
};

/** @struct AudioBatchProducerStats
 *  @brief Statistics collected during production
 */
struct AudioBatchProducerStats
{
  size_t total_batches = 0;
  double elapsed_seconds = 0.0;
  double mean_interval_us = 0.0;
  double stdev_interval_us = 0.0;
  double min_interval_us = 0.0;
  double max_interval_us = 0.0;
  double throughput_hz = 0.0;
  double target_throughput_hz = 0.0;

  std::string to_string() const
  {
    std::ostringstream oss;
    oss << "AudioBatchProducerStats:\n"
        << "  Total batches: " << total_batches << "\n"
        << "  Elapsed time: " << elapsed_seconds << " s\n"
        << "  Batch interval - mean: " << mean_interval_us << " us, stdev: " << stdev_interval_us 
        << " us, min: " << min_interval_us << " us, max: " << max_interval_us << " us\n"
        << "  Throughput: " << throughput_hz << " Hz (target: " << target_throughput_hz << " Hz)";
    return oss.str();
  }
};

/** @class AudioBatchProducer
 *  @brief Produces audio samples in batches at a constant sample rate
 *  
 *  This class simulates an audio callback by pushing batches of samples into a
 *  lock-free ring buffer at precise timing intervals based on the sample rate.
 *  It maintains constant timing by sleeping until target times for each batch.
 *  
 *  @tparam T The type of audio samples (e.g., int, float)
 *  @tparam RingBufferSize The capacity of the target ring buffer
 */
template <typename T, size_t RingBufferSize>
class AudioBatchProducer
{
public:
  using RingBufferType = LockfreeRingBuffer<T, RingBufferSize>;
  using CompletionCallback = std::function<void(const AudioBatchProducerStats&)>;

  /** @brief Constructor
   *  @param ring_buffer Reference to the target ring buffer
   *  @param sample_data Reference to the sample data to produce
   *  @param config Configuration parameters
   */
  AudioBatchProducer(RingBufferType& ring_buffer, 
                     const std::vector<T>& sample_data,
                     const AudioBatchProducerConfig& config = AudioBatchProducerConfig())
    : m_ring_buffer(ring_buffer)
    , m_sample_data(sample_data)
    , m_config(config)
    , m_is_running(false)
    , m_done_producing(false)
  {
    m_time_per_sample_us = 1000000.0 / m_config.sample_rate;
    m_time_per_batch_us = m_time_per_sample_us * m_config.batch_size * m_config.batch_time_multiplier;
  }

  ~AudioBatchProducer()
  {
    stop();
  }

  /** @brief Start the producer thread */
  void start()
  {
    if (m_is_running.load())
      return;

    m_is_running.store(true);
    m_done_producing.store(false);
    m_thread = std::jthread(&AudioBatchProducer::run, this);
  }

  /** @brief Stop the producer thread */
  void stop()
  {
    if (!m_is_running.load())
      return;

    m_is_running.store(false);
    if (m_thread.joinable())
      m_thread.join();
  }

  /** @brief Check if production is complete */
  bool is_done() const
  {
    return m_done_producing.load();
  }

  /** @brief Check if the producer is running */
  bool is_running() const
  {
    return m_is_running.load();
  }

  /** @brief Get the collected statistics
   *  @note Should only be called after production is complete
   */
  AudioBatchProducerStats get_stats() const
  {
    return m_stats;
  }

  /** @brief Set a callback to be invoked when production completes */
  void set_completion_callback(CompletionCallback callback)
  {
    m_completion_callback = callback;
  }

private:
  void run()
  {
    set_thread_name("AudioBatchProducer");

    const size_t num_batches = (m_sample_data.size() + m_config.batch_size - 1) / m_config.batch_size;
    LOG_DEBUG("AudioBatchProducer: Producing ", m_sample_data.size(), " total samples in ", num_batches, 
              " batches of ", m_config.batch_size, " samples.");
    LOG_DEBUG("AudioBatchProducer: Time per batch: ", m_time_per_batch_us, " us (", 
              m_time_per_batch_us / 1000.0, " ms). Expected duration: ",
              (num_batches * m_time_per_batch_us) / 1000000.0, " seconds.");

    std::vector<double> batch_interval_times_us;
    auto start_time = std::chrono::high_resolution_clock::now();
    auto last_batch_time = start_time;
    
    size_t batch_num = 0;
    for (size_t i = 0; i < m_sample_data.size() && m_is_running.load(); i += m_config.batch_size)
    {
      // Calculate when this batch should be pushed
      auto target_time = start_time + std::chrono::duration<double, std::micro>(batch_num * m_time_per_batch_us);

      // Sleep until the target time
      auto now = std::chrono::high_resolution_clock::now();
      if (target_time > now)
      {
        std::this_thread::sleep_until(target_time);
      }

      auto batch_start = std::chrono::high_resolution_clock::now();

      // Determine actual batch size (last batch might be smaller)
      size_t current_batch_size = std::min(m_config.batch_size, m_sample_data.size() - i);
      
      // Push batch of samples
      LOG_DEBUG("AudioBatchProducer: Producing batch ", batch_num, " (samples ", i, " to ", i + current_batch_size - 1, ")");
      
      for (size_t j = 0; j < current_batch_size; ++j)
      {
        // Push the sample (with retry if buffer is full)
        while (!m_ring_buffer.try_push(m_sample_data[i + j]) && m_is_running.load())
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
    m_done_producing.store(true);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    LOG_DEBUG("AudioBatchProducer: Completed producing ", batch_num, " batches in ", elapsed.count(), " seconds.");

    // Calculate statistics
    calculate_stats(batch_interval_times_us, batch_num, elapsed.count());

    // Invoke completion callback if set
    if (m_completion_callback)
    {
      m_completion_callback(m_stats);
    }

    m_is_running.store(false);
  }

  void calculate_stats(const std::vector<double>& batch_interval_times_us, size_t num_batches, double elapsed_seconds)
  {
    m_stats.total_batches = num_batches;
    m_stats.elapsed_seconds = elapsed_seconds;

    if (!batch_interval_times_us.empty())
    {
      double sum = std::accumulate(batch_interval_times_us.begin(), batch_interval_times_us.end(), 0.0);
      m_stats.mean_interval_us = sum / batch_interval_times_us.size();

      std::vector<double> diff(batch_interval_times_us.size());
      std::transform(batch_interval_times_us.begin(), batch_interval_times_us.end(), diff.begin(),
                     [this](double x) { return x - m_stats.mean_interval_us; });
      double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
      m_stats.stdev_interval_us = std::sqrt(sq_sum / batch_interval_times_us.size());

      m_stats.min_interval_us = *std::min_element(batch_interval_times_us.begin(), batch_interval_times_us.end());
      m_stats.max_interval_us = *std::max_element(batch_interval_times_us.begin(), batch_interval_times_us.end());
    }

    m_stats.throughput_hz = static_cast<double>(m_sample_data.size()) / elapsed_seconds;
    m_stats.target_throughput_hz = 1000000.0 / (m_time_per_batch_us / m_config.batch_size);

    LOG_DEBUG(m_stats.to_string());
  }

  RingBufferType& m_ring_buffer;
  const std::vector<T>& m_sample_data;
  AudioBatchProducerConfig m_config;
  AudioBatchProducerStats m_stats;

  std::jthread m_thread;
  std::atomic<bool> m_is_running;
  std::atomic<bool> m_done_producing;

  double m_time_per_sample_us;
  double m_time_per_batch_us;

  CompletionCallback m_completion_callback;
};

} // namespace MinimalAudioEngine

#endif // __AUDIO_BATCH_PRODUCER_H__

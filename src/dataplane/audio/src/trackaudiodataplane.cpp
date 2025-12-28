#include "trackaudiodataplane.h"

using namespace MinimalAudioEngine;

void TrackAudioDataPlane::process_audio(void *output_buffer, void *input_buffer, unsigned int n_frames,
                                        double stream_time, RtAudioStreamStatus status) noexcept
{
  if (input_buffer != nullptr)
  {
    float *in = static_cast<float *>(input_buffer);
    // TODO - Process input buffer: read to input ring buffer
  }

  if (output_buffer == nullptr)
    return;

  float *out = static_cast<float *>(output_buffer);

  // Check stop command once at start
  if (m_stop_command.load(std::memory_order_acquire))
  {
    std::fill_n(out, n_frames * m_output_channels, 0.0f);
    return;
  }

  // Allocate temp buffer once
  float input_samples[8];

  auto batch_start_time = std::chrono::high_resolution_clock::now();

  for (unsigned int i = 0; i < n_frames; ++i)
  {
    // Read frame from ring buffer
    bool buffer_underrun = false;
    for (unsigned int ch = 0; ch < m_input_channels; ++ch)
    {
      if (!p_output_buffer->try_pop(input_samples[ch]))
      {
        buffer_underrun = true;
        break;
      }
    }

    float *frame_out = out + (i * m_output_channels);

    if (buffer_underrun)
    {
      m_audio_output_stats.underrun_count++;
      std::fill_n(frame_out, m_output_channels, 0.0f);
      continue;
    }

    // Channel mapping (optimized)
    if (m_input_channels == 1 && m_output_channels > 1)
    {
      std::fill_n(frame_out, m_output_channels, input_samples[0]);
    }
    else
    {
      for (unsigned int ch = 0; ch < m_output_channels; ++ch)
      {
        frame_out[ch] = (ch < m_input_channels) ? input_samples[ch] : 0.0f;
      }
    }
  }

  auto batch_end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> batch_duration = batch_end_time - batch_start_time;

  double batch_time_ms = batch_duration.count();

  // Update statistics
  m_audio_output_stats.total_frames_read += n_frames;
  m_audio_output_stats.total_batches++;
  m_audio_output_stats.total_read_time_ms += batch_time_ms;
  m_audio_output_stats.batch_size_frames = n_frames;

  // Calculate instantaneous throughput for this batch
  double current_throughput = static_cast<double>(m_audio_output_stats.total_frames_read) /
    (stream_time > 0.0 ? stream_time : 1.0);
  
  // Update min/max throughput
  if (m_audio_output_stats.total_batches == 1)
  {
    m_audio_output_stats.min_batch_time_ms = batch_time_ms;
    m_audio_output_stats.max_batch_time_ms = batch_time_ms;
  }
  else
  {
    m_audio_output_stats.min_batch_time_ms = std::min(m_audio_output_stats.min_batch_time_ms, batch_time_ms);
    m_audio_output_stats.max_batch_time_ms = std::max(m_audio_output_stats.max_batch_time_ms, batch_time_ms);
  }
  
  // Update average throughput (cumulative moving average)
  m_audio_output_stats.throughput_frames_per_second = current_throughput;
}

void TrackAudioDataPlane::read_wav_file(const WavFilePtr& wav_file, ReadWavFileCompleteCallback callback)
{
  LOG_DEBUG("TrackAudioDataPlane: Starting to read WAV file: ", wav_file->to_string());

  // Ensure any existing producer thread is stopped
  if (m_producer_thread.joinable())
  {
    m_stop_command.store(true, std::memory_order_release);
    m_producer_thread.join();
    m_stop_command.store(false, std::memory_order_release);
  }

  // Create producer thread to read WAV file and push data into input buffer
  m_producer_thread = std::jthread([this, wav_file, callback]()
  {
    set_thread_name("WavFileReader");

    // Get sample rate from WAV file
    unsigned int sample_rate = wav_file->get_sample_rate();
    unsigned int channels = wav_file->get_channels();

    // Read frames from WAV file in batches matching the buffer size
    const unsigned int frames_per_read = BUFFER_FRAMES / channels;

    // Calculate time per batch based on the sample rate (real-time delivery)
    double time_per_batch_ms = (static_cast<double>(frames_per_read) / static_cast<double>(sample_rate)) * 1'000.0 * 0.95; // Slightly faster than real-time to avoid underruns

    LOG_DEBUG("TrackAudioDataPlane: WAV file reader thread started. Sample Rate: ", sample_rate,
              ", Channels: ", channels,
              ", Frames per read: ", frames_per_read,
              ", Time per batch (ms): ", time_per_batch_ms);

    sf_count_t total_frames_read = 0;
    std::vector<double> batch_interval_times_ms;

    auto start_time = std::chrono::high_resolution_clock::now();
    auto last_batch_time = start_time;

    std::vector<float> read_buffer(frames_per_read * channels);
    size_t batch_num = 0;
    while (!m_stop_command.load(std::memory_order_acquire))
    {
      auto target_time = start_time + std::chrono::duration<double, std::milli>(batch_num * time_per_batch_ms);

      // Sleep until target time to simulate real-time reading
      // For first few batches, run immediately to fill buffer quickly
      auto now = std::chrono::high_resolution_clock::now();
      if (target_time > now)
      {
        std::this_thread::sleep_until(target_time);
      }

      std::fill_n(read_buffer.data(), read_buffer.size(), 0.0f);

      sf_count_t frames_read = wav_file->read_frames(read_buffer, frames_per_read);
      total_frames_read += frames_read;
      if (frames_read <= 0)
      {
        // End of file reached
        m_stop_command.store(true, std::memory_order_release);
        break;
      }

      // Push read samples into the input buffer
      for (unsigned int i = 0; i < static_cast<unsigned int>(frames_read * channels); ++i)
      {
        // Try to push sample into buffer
        // If buffer is full, skip this sample to maintain timing (rare with proper buffer size)
        if (!p_output_buffer->try_push(read_buffer[i]))
        {
          // Buffer overflow - consumer isn't keeping up
          // Log once per batch to avoid spam
          if (i == 0)
          {
            LOG_WARNING("TrackAudioDataPlane: Ring buffer full, dropping samples (batch ", batch_num, ")");
            m_wav_file_read_stats.overrun_count += frames_read;
          }
        }
      }

      auto batch_end = std::chrono::high_resolution_clock::now();

      // Measure actual interval from last batch
      if (batch_num > 0)
      {
        std::chrono::duration<double, std::milli> interval = now - last_batch_time;
        batch_interval_times_ms.push_back(interval.count());
      }

      last_batch_time = batch_end;
      ++batch_num;
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    m_wav_file_read_stats.total_frames_read = total_frames_read;
    m_wav_file_read_stats.total_read_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    m_wav_file_read_stats.batch_size_frames = frames_per_read;
    m_wav_file_read_stats.total_batches = batch_num;
    m_wav_file_read_stats.max_batch_time_ms = batch_interval_times_ms.empty() ? 0.0 : *std::max_element(batch_interval_times_ms.begin(), batch_interval_times_ms.end());
    m_wav_file_read_stats.min_batch_time_ms = batch_interval_times_ms.empty() ? 0.0 : *std::min_element(batch_interval_times_ms.begin(), batch_interval_times_ms.end());

    // Calculate average batch interval time
    double total_interval = 0.0;
    for (const auto& interval : batch_interval_times_ms)
    {
      total_interval += interval;
    }
    m_wav_file_read_stats.average_batch_time_ms = batch_interval_times_ms.empty() ? 0.0 : total_interval / static_cast<double>(batch_interval_times_ms.size());
    m_wav_file_read_stats.throughput_frames_per_second = static_cast<double>(total_frames_read) / ((end_time - start_time).count() / 1'000'000'000.0);

    if (callback)
    {
      callback(m_wav_file_read_stats);
    }
  });
}
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

  auto batch_start_time = std::chrono::high_resolution_clock::now();

  // Fill output buffer from preloaded frames buffer
  unsigned int read_pos = m_read_position.load(std::memory_order_acquire);
  for (unsigned int i = 0; i < n_frames; ++i)
  {
    for (unsigned int ch = 0; ch < m_output_channels; ++ch)
    {
      unsigned int buffer_index = (read_pos + i) * m_output_channels + ch;
      if (buffer_index < m_preloaded_frames_buffer.size())
      {
        out[i * m_output_channels + ch] = m_preloaded_frames_buffer[buffer_index];
      }
      else
      {
        out[i * m_output_channels + ch] = 0.0f; // Fill with silence if out of preloaded data
      }
    }
  }

  auto batch_end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> batch_duration = batch_end_time - batch_start_time;

  double batch_time_ms = batch_duration.count();

  // Advance read position
  m_read_position.fetch_add(n_frames, std::memory_order_release);

  // Update statistics
  update_audio_output_statistics(n_frames, batch_time_ms, stream_time);
}

void TrackAudioDataPlane::read_wav_file(const WavFilePtr& wav_file)
{
  if (!wav_file)
  {
    LOG_ERROR("TrackAudioDataPlane: Invalid WAV file: ", wav_file->to_string());
    return;
  }

  LOG_INFO("TrackAudioDataPlane: Preloading WAV file: ", wav_file->to_string());

  wav_file->seek(0);
  m_read_position.store(0, std::memory_order_release);

  // Read entire WAV file into preloaded buffer
  sf_count_t total_frames = wav_file->get_total_frames(); // Expect interleaved samples
  sf_count_t total_samples = total_frames * wav_file->get_channels();
  m_preloaded_frames_buffer.resize(total_samples, 0.0f);
  sf_count_t frames_read = wav_file->read_frames(m_preloaded_frames_buffer, total_samples);

  if (frames_read != total_samples)
  {
    LOG_WARNING("TrackAudioDataPlane: Read fewer frames than expected from WAV file: ",
                frames_read, " / ", total_samples);
  }
}

void TrackAudioDataPlane::update_audio_output_statistics(unsigned int n_frames, double batch_time_ms, double stream_time)
{
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

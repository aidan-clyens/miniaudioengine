#include "audiodataplane.h"

using namespace miniaudioengine::data;
using namespace miniaudioengine::file;
using namespace miniaudioengine::core;

void AudioDataPlane::process_audio(void *output_buffer, void *input_buffer, unsigned int n_frames,
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

  // Check if stopped once before processing buffer
  if (!is_running())
  {
    std::fill_n(out, n_frames * m_output_channels, 0.0f);
    return;
  }

  auto batch_start_time = std::chrono::high_resolution_clock::now();

  // Prepare output buffer for this track
  prepare_output_buffer(n_frames);

  // Fill output buffer from preloaded frames buffer
  unsigned int read_pos = m_read_position.load(std::memory_order_acquire);
  for (unsigned int i = 0; i < n_frames; ++i)
  {
    for (unsigned int ch = 0; ch < m_output_channels; ++ch)
    {
      unsigned int buffer_index = (read_pos + i) * m_output_channels + ch;
      if (buffer_index < m_preloaded_frames_buffer.size())
      {
        float sample = m_preloaded_frames_buffer[buffer_index];
        m_output_buffer[i * m_output_channels + ch] = sample; // Write to virtual output
        out[i * m_output_channels + ch] = sample; // Write to hardware output
      }
      else
      {
        m_output_buffer[i * m_output_channels + ch] = 0.0f;
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

/** @brief Preload WAV file data into the audio data plane.
 *  @param wav_file Shared pointer to the WavFile to preload.
 */
void AudioDataPlane::preload_wav_file(const WavFilePtr& wav_file)
{
  if (!wav_file)
  {
    LOG_ERROR("AudioDataPlane: Invalid WAV file: ", wav_file->to_string());
    return;
  }

  // Only preload if track is not running
  if (is_running())
  {
    LOG_WARNING("AudioDataPlane: Cannot preload WAV file while track is running.");
    return;
  }

  LOG_INFO("AudioDataPlane: Preloading WAV file: ", wav_file->to_string());

  wav_file->seek(0);
  m_read_position.store(0, std::memory_order_release);

  // Read entire WAV file into preloaded buffer
  sf_count_t total_frames = wav_file->get_total_frames(); // Expect interleaved samples
  sf_count_t total_samples = total_frames * wav_file->get_channels();
  m_preloaded_frames_buffer.resize(total_samples, 0.0f);
  sf_count_t frames_read = wav_file->read_frames(m_preloaded_frames_buffer, total_samples);

  if (frames_read != total_frames)
  {
    LOG_WARNING("AudioDataPlane: Read fewer frames than expected from WAV file: ",
                frames_read, " / ", total_frames);
  }
}

void AudioDataPlane::update_audio_output_statistics(unsigned int n_frames, double batch_time_ms, double stream_time)
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

// ============================================================================
// Mixing and Routing for Hierarchy
// ============================================================================

/** @brief Prepare output buffer for mixing (called before processing children).
 *  @param n_frames Number of frames to prepare.
 */
void AudioDataPlane::prepare_output_buffer(unsigned int n_frames)
{
  size_t buffer_size = n_frames * m_output_channels;
  if (m_output_buffer.size() != buffer_size)
  {
    m_output_buffer.resize(buffer_size, 0.0f);
  }
  
  // Clear output buffer before mixing
  std::fill(m_output_buffer.begin(), m_output_buffer.end(), 0.0f);
}

/** @brief Mix child track output into this track's output buffer.
 *  @param child_dataplane The child track's data to read from.
 *  @param child_gain Gain to apply to child output.
 *  @param n_frames Number of frames to mix.
 *  @note This is called in the parent track's audio callback (data plane).
 */
void AudioDataPlane::mix_child_output(const AudioDataPlane& child_dataplane,
                                      float child_gain,
                                      unsigned int n_frames) noexcept
{
  const auto& child_buffer = child_dataplane.get_output_buffer();
  
  // Ensure output buffer is allocated
  size_t expected_size = n_frames * m_output_channels;
  if (m_output_buffer.size() < expected_size)
  {
    return; // Skip mixing if buffer size mismatch
  }

  // Mix child output into parent's output buffer with gain
  size_t mix_samples = std::min(child_buffer.size(), expected_size);
  for (size_t i = 0; i < mix_samples; ++i)
  {
    m_output_buffer[i] += child_buffer[i] * child_gain;
  }
}

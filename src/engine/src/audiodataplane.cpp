#include "audiodataplane.h"

using namespace miniaudioengine::audio;
using namespace miniaudioengine;
using namespace miniaudioengine::core;

void AudioDataPlane::process_audio(void *output_buffer, void *input_buffer, unsigned int n_frames,
                                        double stream_time, RtAudioStreamStatus status) noexcept
{
  (void)status; // Unused for now, but can be used to handle underflow/overflow conditions in the future
  float *in_buffer = static_cast<float *>(input_buffer);
  float *out_buffer = static_cast<float *>(output_buffer);

  prepare_output_buffer(n_frames);

  // Exit if dataplane is stopped
  if (!is_running())
  {
    return;
  }

  auto batch_start_time = std::chrono::high_resolution_clock::now();

  // Step 1: Read input buffer (if any)
  // If input buffer is defined, then input is comign from a device
  // If not, check for file input (preloaded buffer)
  if (in_buffer != nullptr)
  {
    // Copy from input buffer to output buffer
    // TODO - Handle difference in input/output channels
    for (size_t i = 0; i < n_frames * m_input_channels; ++i)
    {
      m_output_buffer[i] = in_buffer[i];
    }
  }
  else if (!m_preloaded_frames_buffer.empty())
  {
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
          m_output_buffer[i * m_output_channels + ch] = sample; // Write to hardware output
        }
        else
        {
          m_output_buffer[i * m_output_channels + ch] = 0.0f; // Fill with silence if out of preloaded data
        }
      }
    }

    // Advance read position
    m_read_position.fetch_add(n_frames, std::memory_order_release);
  }

  // Step 2: Apply processing chain
  for (auto &processor : m_processors)
  {
    if (processor->is_bypassed())
    {
      continue; // Skip processing if bypassed
    }

    processor->process_audio(m_output_buffer.data(), m_output_channels, n_frames, stream_time);
  }

  // Step 3: Write to output buffer (if provided)
  if (out_buffer != nullptr)
  {
    for (unsigned int i = 0; i < n_frames * m_output_channels; ++i)
    {
      out_buffer[i] = m_output_buffer[i];
    }
  }

  // Step 4: Update statistics
  auto batch_end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> batch_duration = batch_end_time - batch_start_time;
  double batch_time_ms = batch_duration.count();

  // Update statistics
  update_audio_output_statistics(n_frames, batch_time_ms, stream_time);
}

/** @brief Preload audio file data into the audio data plane.
 *  @param file_handle Shared pointer to a FileHandle (must be eFileType::Wav).
 */
void AudioDataPlane::preload_wav_file(const FileHandlePtr& file_handle)
{
  if (!file_handle)
  {
    LOG_ERROR("AudioDataPlane: Null file handle passed to preload_wav_file.");
    return;
  }

  // Only preload if track is not running
  if (is_running())
  {
    LOG_WARNING("AudioDataPlane: Cannot preload WAV file while track is running.");
    return;
  }

  LOG_INFO("AudioDataPlane: Preloading audio file: ", file_handle->to_string());

  file_handle->seek(0);
  m_read_position.store(0, std::memory_order_release);

  // Read entire file into preloaded buffer
  long long total_frames  = static_cast<long long>(file_handle->get_total_frames());
  long long total_samples = total_frames * static_cast<long long>(file_handle->get_channels());
  m_preloaded_frames_buffer.resize(static_cast<size_t>(total_samples), 0.0f);
  long long frames_read = file_handle->read_frames(m_preloaded_frames_buffer, total_samples);

  if (frames_read != total_frames)
  {
    LOG_WARNING("AudioDataPlane: Read fewer frames than expected from audio file: ",
                frames_read, " / ", total_frames);
  }
}

void AudioDataPlane::update_audio_output_statistics(unsigned int n_frames, double batch_time_ms, double stream_time)
{
  // Update statistics
  if (!p_statistics)
  {
    LOG_WARNING("AudioDataPlane: No statistics object available to update.");
    return;
  }

  auto audio_stats = std::dynamic_pointer_cast<AudioOutputStatistics>(p_statistics);
  if (!audio_stats)
  {
    LOG_WARNING("AudioDataPlane: Statistics object is not of type AudioOutputStatistics.");
    return;
  }

  audio_stats->total_frames_read += n_frames;
  audio_stats->total_batches++;
  audio_stats->total_read_time_ms += batch_time_ms;
  audio_stats->batch_size_frames = n_frames;

  // Calculate instantaneous throughput for this batch
  double current_throughput = static_cast<double>(audio_stats->total_frames_read) /
    (stream_time > 0.0 ? stream_time : 1.0);

  // Update min/max throughput
  if (audio_stats->total_batches == 1)
  {
    audio_stats->min_batch_time_ms = batch_time_ms;
    audio_stats->max_batch_time_ms = batch_time_ms;
  }
  else
  {
    audio_stats->min_batch_time_ms = std::min(audio_stats->min_batch_time_ms, batch_time_ms);
    audio_stats->max_batch_time_ms = std::max(audio_stats->max_batch_time_ms, batch_time_ms);
  }

  // Update average throughput (cumulative moving average)
  audio_stats->throughput_frames_per_second = current_throughput;
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

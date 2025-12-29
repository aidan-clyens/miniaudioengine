#ifndef __TRACK_AUDIO_DATA_PLANE_H__
#define __TRACK_AUDIO_DATA_PLANE_H__

#include "lockfree_ringbuffer.h"

#include <string>
#include <memory>
#include <thread>
#include <functional>
#include <algorithm>
#include <vector>

#include <rtaudio/RtAudio.h>

#include "wavfile.h"
#include "logger.h"

#define BUFFER_FRAMES 8192

namespace MinimalAudioEngine
{

/** @struct AudioOutputStatistics
 *  @brief Statistics related to reading WAV files.
 */
struct AudioOutputStatistics
{
  sf_count_t total_frames_read = 0;
  double total_read_time_ms = 0.0;
  size_t batch_size_frames = 0;
  size_t total_batches = 0;
  double average_batch_time_ms = 0.0;
  double max_batch_time_ms = 0.0;
  double min_batch_time_ms = 0.0;
  double throughput_frames_per_second = 0.0;
  size_t underrun_count = 0;
  size_t overrun_count = 0;

  AudioOutputStatistics() = default;
  AudioOutputStatistics(const AudioOutputStatistics&) = default;
  AudioOutputStatistics& operator=(const AudioOutputStatistics&) = default;
  virtual ~AudioOutputStatistics() = default;

  std::string to_string() const
  {
    return "AudioOutputStatistics(\n  Total Frames Read = " + std::to_string(total_frames_read) +
           "\n  Total Read Time = " + std::to_string(total_read_time_ms) + " ms" +
           "\n  Batch Size Frames = " + std::to_string(batch_size_frames) +
           "\n  Total Batches = " + std::to_string(total_batches) +
           "\n  Average Batch Time = " + std::to_string(average_batch_time_ms) + " ms" +
           "\n  Max Batch Time = " + std::to_string(max_batch_time_ms) + " ms" +
           "\n  Min Batch Time = " + std::to_string(min_batch_time_ms) + " ms" +
           "\n  Throughput = " + std::to_string(throughput_frames_per_second) + " Hz" +
           "\n  Underrun Count = " + std::to_string(underrun_count) + " frames" +
           "\n  Overrun Count = " + std::to_string(overrun_count) + " frames)";
  }
};

/** @class TrackAudioDataPlane
 *  @brief Data plane for handling audio data for a Track.
 */
class TrackAudioDataPlane
{
public:
  using LockfreeRingBuffer = MinimalAudioEngine::LockfreeRingBuffer<float, BUFFER_FRAMES>;
  using LockfreeRingBufferPtr = std::shared_ptr<LockfreeRingBuffer>;

  using ReadWavFileCompleteCallback = std::function<void(const AudioOutputStatistics&)>;

  TrackAudioDataPlane() : 
    p_output_buffer(std::make_shared<LockfreeRingBuffer>())
  {}

  virtual ~TrackAudioDataPlane()
  {
    stop();
  }

  /** @brief Set the number of input channels of the audio input.
   *  @param channels Number of input channels.
   */
  void set_input_channels(unsigned int channels)
  {
    m_input_channels = channels;
  }

  /** @brief Set the number of output channels of the audio output.
   *  @param channels Number of output channels.
   */
  void set_output_channels(unsigned int channels)
  {
    m_output_channels = channels;
  }

  /** @brief Get the number of input channels.
   *  @return Number of input channels.
   */
  inline unsigned int get_input_channels() const
  {
    return m_input_channels;
  }

  /** @brief Get the number of output channels.
   *  @return Number of output channels.
   */
  inline unsigned int get_output_channels() const
  {
    return m_output_channels;
  }

  // Called from RtAudio callback
  void process_audio(void *output_buffer, void *input_buffer, unsigned int n_frames,
                     double stream_time, RtAudioStreamStatus status) noexcept;

  // Called from control plane to preload WAV file data
  void read_wav_file(const WavFilePtr& wav_file);

  /** @brief Stop audio processing and clear buffers. */
  void stop()
  {
    m_stop_command.store(true, std::memory_order_release);
    p_output_buffer->clear();
    m_preloaded_frames_buffer.clear();
  }

  /** @brief Get audio output statistics.
   *  @return AudioOutputStatistics structure containing output statistics.
   */
  AudioOutputStatistics get_audio_output_statistics() const
  {
    return m_audio_output_stats;
  }

  /** @brief Get the output lock-free ring buffer.
   *  @return Shared pointer to the output LockfreeRingBuffer.
   */
  LockfreeRingBufferPtr get_output_buffer() const
  {
    return p_output_buffer;
  }

  std::string to_string() const
  {
    return "TrackAudioDataPlane";
  }

private:
  LockfreeRingBufferPtr p_output_buffer;
  std::vector<float> m_preloaded_frames_buffer;

  AudioOutputStatistics m_audio_output_stats;

  std::atomic<bool> m_stop_command{false};
  std::atomic<unsigned int> m_read_position{0};

  unsigned int m_input_channels{0};
  unsigned int m_output_channels{0};

private:
  void update_audio_output_statistics(unsigned int n_frames, double batch_time_ms, double stream_time);
};

typedef std::shared_ptr<TrackAudioDataPlane> TrackAudioDataPlanePtr;

} // namespace MinimalAudioEngine

#endif // __TRACK_AUDIO_DATA_PLANE_H__
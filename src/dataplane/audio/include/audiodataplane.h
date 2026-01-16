#ifndef __TRACK_AUDIO_DATA_PLANE_H__
#define __TRACK_AUDIO_DATA_PLANE_H__

#include <string>
#include <memory>
#include <functional>
#include <algorithm>
#include <vector>

#include <rtaudio/RtAudio.h>

#include "dataplane.h"
#include "wavfile.h"
#include "logger.h"

namespace miniaudioengine::data
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

/** @class AudioDataPlane
 *  @brief Data plane for handling audio data for a Track. The Data plane is only a callback
 *  target for RtAudio and is not a producer/consumer of audio data itself.
 */
class AudioDataPlane : public core::IDataPlane
{
public:
  /** @brief Start audio processing.
   */
  void start() override
  {
    core::IDataPlane::start();
    m_read_position.store(0, std::memory_order_release);
  }

  /** @brief Stop audio processing and clear buffers.
   */
  void stop() override
  {
    core::IDataPlane::stop();
    m_preloaded_frames_buffer.clear();
  }

  /** @brief Process audio data. This is called from the RtAudio callback function.
   *  @param output_buffer Pointer to the output buffer.
   *  @param input_buffer Pointer to the input buffer.
   *  @param n_frames Number of frames to process.
   *  @param stream_time Current stream time in seconds.
   *  @param status Stream status flags.
   */
  void process_audio(void *output_buffer, void *input_buffer, unsigned int n_frames,
                     double stream_time, RtAudioStreamStatus status) noexcept;
  
  /** @brief Preload WAV file data into the audio data plane. Called from the Track control plane before playback.
   *  @param wav_file Shared pointer to the WavFile to preload.
   */
  void preload_wav_file(const control::WavFilePtr& wav_file);

  /** @brief Get audio output statistics.
   *  @return AudioOutputStatistics structure containing output statistics.
   */
  AudioOutputStatistics get_audio_output_statistics() const
  {
    return m_audio_output_stats;
  }

  // Mixing and routing for hierarchy

  /** @brief Mix child track output into this track's output buffer.
   *  @param child_dataplane The child track's dataplane to read from.
   *  @param child_gain Gain to apply to child output.
   *  @param n_frames Number of frames to mix.
   *  @note This is called in the parent track's audio callback (data plane).
   */
  void mix_child_output(const AudioDataPlane& child_dataplane,
                        float child_gain,
                        unsigned int n_frames) noexcept;

  /** @brief Get the output buffer for parent to read.
   *  @return Const reference to output buffer.
   */
  const std::vector<float>& get_output_buffer() const { return m_output_buffer; }

  /** @brief Prepare output buffer for mixing (called before processing children).
   *  @param n_frames Number of frames to prepare.
   */
  void prepare_output_buffer(unsigned int n_frames);

  std::string to_string() const
  {
    return "AudioDataPlane";
  }

private:
  std::vector<float> m_preloaded_frames_buffer;
  std::vector<float> m_output_buffer;  // Virtual output for routing to parent
  std::vector<float> m_mix_buffer;     // Temporary buffer for mixing operations

  AudioOutputStatistics m_audio_output_stats;

  std::atomic<unsigned int> m_read_position{0};

private:
  void update_audio_output_statistics(unsigned int n_frames, double batch_time_ms, double stream_time);
};

typedef std::shared_ptr<AudioDataPlane> TrackAudioDataPlanePtr;

} // namespace miniaudioengine::data

#endif // __TRACK_AUDIO_DATA_PLANE_H__
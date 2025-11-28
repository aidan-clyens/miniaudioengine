#ifndef _AUDIO_INTERFACE_H_
#define _AUDIO_INTERFACE_H_

#include <memory>
#include <atomic>
#include <rtaudio/RtAudio.h>

namespace Audio
{

/** @struct AudioDeviceInfo
 *  @brief Extends RtAudio::DeviceInfo to add additional fields
 */
struct AudioDeviceInfo : public RtAudio::DeviceInfo
{
  // Additional fields can be added here if needed
};

/** @class AudioInterface
 *  @brief Wrapper around RtAudio for audio stream management
 */
class AudioInterface
{
public:
  AudioInterface();
  ~AudioInterface();

  bool open(unsigned int device_id);
  bool start();
  bool close();

  inline void set_channels(unsigned int channels) noexcept
  {
    m_channels.store(channels, std::memory_order_relaxed);
  }

  inline unsigned int get_channels() const noexcept
  {
    return m_channels.load(std::memory_order_relaxed);
  }

  inline void set_sample_rate(unsigned int sample_rate) noexcept
  {
    m_sample_rate.store(sample_rate, std::memory_order_relaxed);
  }

  inline unsigned int get_sample_rate() const noexcept
  {
    return m_sample_rate.load(std::memory_order_relaxed);
  }

  inline void set_buffer_frames(unsigned int buffer_frames) noexcept
  {
    m_buffer_frames.store(buffer_frames, std::memory_order_relaxed);
  }

  inline unsigned int get_buffer_frames() const noexcept
  {
    return m_buffer_frames.load(std::memory_order_relaxed);
  }

  inline unsigned int get_device_count()
  {
    return m_rtaudio.getDeviceCount();
  }

  inline AudioDeviceInfo get_device_info(unsigned int device_id)
  {
    return static_cast<AudioDeviceInfo>(m_rtaudio.getDeviceInfo(device_id));
  }

  inline bool is_stream_running() const
  {
    return m_rtaudio.isStreamRunning();
  }

  void process_audio(float *output_buffer, unsigned int n_frames);

  // Disable copy constructor and assignment operator
  AudioInterface(const AudioInterface & ) = delete;
  AudioInterface & operator=(const AudioInterface & ) = delete;

private:
  RtAudio m_rtaudio;
  std::atomic<bool> m_should_close{false};

  std::atomic<unsigned int> m_channels;
  std::atomic<unsigned int> m_sample_rate;
  std::atomic<unsigned int> m_buffer_frames;
};

} // namespace Audio

#endif // _AUDIO_INTERFACE_H_
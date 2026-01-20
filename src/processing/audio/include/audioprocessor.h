#ifndef __AUDIO_PROCESSOR_H__
#define __AUDIO_PROCESSOR_H__

#include <string>
#include <atomic>

#include "processor.h"

// Forward declaration from rtaudio library
typedef unsigned int RtAudioStreamStatus;

namespace miniaudioengine::processing
{

/** @brief Interface for audio processors. Real application or test mock */
class IAudioProcessor : public core::IProcessor
{
public:
  virtual ~IAudioProcessor() = default;

  /** @brief Process audio data.
   *  @param output_buffer Pointer to the output buffer.
   *  @param channels Number of audio channels.
   *  @param n_frames Number of frames to process.
   *  @param stream_time Current stream time in seconds.
   */
  virtual void process_audio(float *output_buffer, unsigned int channels, unsigned int n_frames, double stream_time) noexcept = 0;

  /** @brief Reset the processor to its initial state.
   */ 
  virtual void reset() = 0;

  /** @brief Set or unset bypass mode.
   *  @param bypass True to enable bypass, false to disable.
   */
  void set_bypass(bool bypass) noexcept
  {
    m_bypass.store(bypass, std::memory_order_release);
  }

  /** @brief Check if the processor is in bypass mode.
   *  @return True if bypass is enabled, false otherwise.
   */
  bool is_bypassed() const noexcept
  {
    return m_bypass.load(std::memory_order_acquire);
  }

  /** @brief Get a string representation of the processor.
   *  @return String representation.
   */
  virtual std::string to_string() const
  {
    return "IAudioProcessor";
  }

private:
  std::atomic<bool> m_bypass{false};
};

};

#endif // __AUDIO_PROCESSOR_H__
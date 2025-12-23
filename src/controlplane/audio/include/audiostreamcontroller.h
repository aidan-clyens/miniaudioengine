#ifndef __AUDIO_STREAM_CONTROLLER_H__
#define __AUDIO_STREAM_CONTROLLER_H__

#include "audiodevice.h"

#include <rtaudio/RtAudio.h>
#include <optional>
#include <vector>

namespace MinimalAudioEngine
{

// Forward declaration - TODO implement later
class AudioCallbackContext;

/** @enum eAudioState
 *  @brief AudioEngine states
 */
enum class eAudioState
{
  Idle,
  Stopped,
  Playing,
};

/** @class AudioStreamController
 *  @brief This class managers the device audio hardware interfaces.
 *  It is responsible for initializing, configuring, and controlling audio inputs and outputs.
 *  The AudioStreamController provides methods to start and stop audio streams, as well as to
 *  adjust parameters such as volume, sample rate, and buffer size. 
 *  @note This class is part of the control plane. Operations are synchronous and called from the main thread.
 */
class AudioStreamController
{
public:
  static AudioStreamController& instance()
  {
    static AudioStreamController instance;
    return instance;
  }

  std::vector<RtAudio::DeviceInfo> get_audio_devices();
  void set_output_device(const AudioDevice& device);

  void start_stream();
  void stop_stream();

  void register_callback_context(const AudioCallbackContext& context); // Set context for RtAudio callback

  eAudioState get_stream_state() const
  {
    return m_stream_state;
  }

private:
  AudioStreamController() = default;
  virtual ~AudioStreamController() = default;

  RtAudio m_rtaudio;
  std::optional<AudioDevice> m_audio_output_device;
  AudioCallbackContext* m_callback_context{nullptr};

  eAudioState m_stream_state{eAudioState::Idle};
};

} // namespace MinimalAudioEngine

#endif // __AUDIO_STREAM_CONTROLLER_H__
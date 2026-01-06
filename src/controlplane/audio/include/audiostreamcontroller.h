#ifndef __AUDIO_STREAM_CONTROLLER_H__
#define __AUDIO_STREAM_CONTROLLER_H__

#include "controller.h"
#include "audiocallbackhandler.h"
#include "audiodevice.h"

#include <rtaudio/RtAudio.h>
#include <optional>
#include <vector>
#include <memory>

namespace MinimalAudioEngine::Control
{

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
class AudioStreamController : public Core::IController
{
public:
  static AudioStreamController& instance()
  {
    static AudioStreamController instance;
    return instance;
  }

  std::vector<RtAudio::DeviceInfo> get_audio_devices();
  void set_output_device(const AudioDevice& device);

  bool start_stream();
  bool stop_stream();

  eAudioState get_stream_state() const
  {
    return m_stream_state;
  }

private:
  AudioStreamController(): m_callback_context(std::make_shared<Data::AudioCallbackContext>()) {}
  virtual ~AudioStreamController() = default;

  RtAudio m_rtaudio;
  std::optional<AudioDevice> m_audio_output_device;
  std::shared_ptr<Data::AudioCallbackContext> m_callback_context;

  eAudioState m_stream_state{eAudioState::Idle};
};

} // namespace MinimalAudioEngine::Control

#endif // __AUDIO_STREAM_CONTROLLER_H__
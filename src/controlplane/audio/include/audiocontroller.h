#ifndef __AUDIO_CONTROLLER_H__
#define __AUDIO_CONTROLLER_H__

#include "controller.h"
#include "audiodevice.h"

#include <vector>
#include <rtaudio/RtAudio.h>

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

/** @class IAudioController
 *  @brief Interface for audio controllers in the framework.
 *  This class serves as a base for all audio-related controllers,
 *  providing a common interface and shared functionality.
 */
class IAudioController : public Core::IController
{
public:
  virtual ~IAudioController() = default;

  virtual std::vector<RtAudio::DeviceInfo> get_audio_devices() = 0;
  virtual void set_output_device(const AudioDevice &device) = 0;
  virtual bool start_stream() = 0;
  virtual bool stop_stream() = 0;
  virtual eAudioState get_stream_state() const = 0;
};

} // namespace MinimalAudioEngine::Control

#endif // __AUDIO_CONTROLLER_H__
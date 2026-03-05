#ifndef __AUDIO_CONTROLLER_H__
#define __AUDIO_CONTROLLER_H__

#include "audiocontroller_interface.h"
#include "audiocallbackhandler.h"
#include "audiodevice.h"

#include <rtaudio/RtAudio.h>
#include <optional>
#include <vector>
#include <memory>

namespace miniaudioengine::audio
{

/** @class AudioController
 *  @brief This class managers the device audio hardware interfaces.
 *  It is responsible for initializing, configuring, and controlling audio inputs and outputs.
 *  The AudioController provides methods to start and stop audio streams, as well as to
 *  adjust parameters such as volume, sample rate, and buffer size.
 *  @note This class is part of the control plane. Operations are synchronous and called from the main thread.
 */
class AudioController : public IAudioController
{
public:
  explicit AudioController() = default;
  ~AudioController() override = default;

  /** @brief Get Available Audio Devices
   *  @return List of available audio devices
   */
  std::vector<core::IAudioDevicePtr> get_audio_devices();

private:
  RtAudio m_rtaudio;

  /** @brief Start Audio Stream
   *  @return true if the audio stream was started successfully, false otherwise
   */
  bool _start() override;

  /** @brief Stop Audio Stream
   *  @return true if the audio stream was stopped successfully, false otherwise
   */
  bool _stop() override;
};

using AudioControllerPtr = std::shared_ptr<AudioController>;

} // namespace miniaudioengine::audio

#endif // __AUDIO_CONTROLLER_H__

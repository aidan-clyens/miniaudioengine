#ifndef __AUDIO_STREAM_CONTROLLER_H__
#define __AUDIO_STREAM_CONTROLLER_H__

#include "audiocontroller.h"
#include "audiocallbackhandler.h"
#include "audiodevice.h"

#include <rtaudio/RtAudio.h>
#include <optional>
#include <vector>
#include <memory>

namespace miniaudioengine::control
{

/** @class AudioStreamController
 *  @brief This class managers the device audio hardware interfaces.
 *  It is responsible for initializing, configuring, and controlling audio inputs and outputs.
 *  The AudioStreamController provides methods to start and stop audio streams, as well as to
 *  adjust parameters such as volume, sample rate, and buffer size. 
 *  @note This class is part of the control plane. Operations are synchronous and called from the main thread.
 */
class AudioStreamController : public IAudioController
{
public:
  explicit AudioStreamController() = default;
  ~AudioStreamController() override = default;

  /** @brief Get Available Audio Devices
   *  @return List of available audio devices
   */
  std::vector<RtAudio::DeviceInfo> get_audio_devices() override;

  /** @brief Set Output Audio Device
   *  @param device Audio device to set as output
   */
  void set_output_device(const AudioDevice& device) override;

  /** @brief Start Audio Stream
   *  @return true if the audio stream was started successfully, false otherwise
   */
  bool start_stream() override;
  
  /** @brief Stop Audio Stream
   *  @return true if the audio stream was stopped successfully, false otherwise
   */
  bool stop_stream() override;

private:
  RtAudio m_rtaudio;
};

} // namespace miniaudioengine::control

#endif // __AUDIO_STREAM_CONTROLLER_H__
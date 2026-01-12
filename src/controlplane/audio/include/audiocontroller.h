#ifndef __AUDIO_CONTROLLER_H__
#define __AUDIO_CONTROLLER_H__

#include "controller.h"
#include "audiodevice.h"
#include "audiocallbackhandler.h"
#include "logger.h"

#include <vector>
#include <optional>
#include <memory>
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
 *  @brief Abstract base class for audio controllers in the framework.
 *  This class provides common state management and validation logic
 *  for all audio-related controllers. Concrete implementations handle
 *  hardware-specific operations.
 */
class IAudioController : public Core::IController
{
public:
  virtual ~IAudioController() = default;

  virtual std::vector<RtAudio::DeviceInfo> get_audio_devices() = 0;
  virtual void set_output_device(const AudioDevice &device) = 0;

  virtual std::optional<AudioDevice> get_output_device() const
  {
    return m_audio_output_device;
  }

  virtual bool start_stream() = 0;
  virtual bool stop_stream() = 0;

  virtual eAudioState get_stream_state() const
  {
    return m_stream_state;
  }

  virtual std::shared_ptr<Data::AudioCallbackContext> get_callback_context() const
  {
    return m_callback_context;
  }

protected:
  IAudioController() : m_callback_context(std::make_shared<Data::AudioCallbackContext>()) {}

  /** @brief Validates preconditions before starting the audio stream.
   *  @return true if all preconditions are met, false otherwise.
   */
  bool validate_start_preconditions() const;

  /** @brief Registers active tracks from TrackManager for audio callbacks.
   *  @return true if tracks were successfully registered, false if no active tracks.
   */
  bool register_dataplanes();

  // Common state shared by all implementations
  std::optional<AudioDevice> m_audio_output_device;
  std::shared_ptr<Data::AudioCallbackContext> m_callback_context;
  eAudioState m_stream_state{eAudioState::Idle};
};

} // namespace MinimalAudioEngine::Control

#endif // __AUDIO_CONTROLLER_H__
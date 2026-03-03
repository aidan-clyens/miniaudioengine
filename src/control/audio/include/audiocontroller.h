#ifndef __AUDIO_CONTROLLER_H__
#define __AUDIO_CONTROLLER_H__

#include "controller.h"
#include "audiodevice.h"
#include "audiocallbackhandler.h"
#include "logger.h"

#include <vector>
#include <optional>
#include <memory>

namespace miniaudioengine::audio
{

/** @class IAudioController
 *  @brief Abstract base class for audio controllers in the framework.
 *  This class provides common state management and validation logic
 *  for all audio-related controllers. Concrete implementations handle
 *  hardware-specific operations.
 *  @deprecated This class is deprecated and will be removed in a future release. Please use AudioStreamController instead.
 */
class IAudioController : public core::IController
{
public:
  virtual ~IAudioController() = default;

  virtual std::vector<core::IAudioDevicePtr> get_audio_devices() = 0;

  virtual std::shared_ptr<core::AudioCallbackContext> get_callback_context() const
  {
    return m_callback_context;
  }

protected:
  IAudioController():
    core::IController("IAudioController"),
    m_callback_context(std::make_shared<core::AudioCallbackContext>()) {}

  /** @brief Validates preconditions before starting the audio stream.
   *  @return true if all preconditions are met, false otherwise.
   */
  bool validate_start_preconditions() const;

  /** @brief Registers active tracks from TrackManager for audio callbacks.
   *  @return true if tracks were successfully registered, false if no active tracks.
   */
  bool register_dataplanes();

  // Common state shared by all implementations
  std::shared_ptr<core::AudioCallbackContext> m_callback_context;
};

} // namespace miniaudioengine::audio

#endif // __AUDIO_CONTROLLER_H__
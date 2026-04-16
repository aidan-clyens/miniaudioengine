#ifndef __AUDIO_CONTROLLER_H__
#define __AUDIO_CONTROLLER_H__

#include "interfaces/controller.h"
#include "devicehandle_factory.h"
#include "audioadapter.h"
#include "logger.h"

#include <optional>
#include <vector>
#include <memory>

namespace miniaudioengine::audio
{

struct AudioCallbackContext
{
  std::vector<core::IDataPlanePtr> active_tracks;
};

/** @class AudioController
 *  @brief Abstract base class and primary implementation for audio controllers.
 *  Provides common state management, device validation, and dataplane registration
 *  logic shared by all audio controller implementations. Concrete subclasses supply
 *  hardware-specific stream start/stop behaviour.
 *  @note This class is part of the control plane. Operations are synchronous and called from the main thread.
 */
class AudioController : public core::IController
{
public:
  explicit AudioController();
  ~AudioController() override = default;

  /** @brief Enumerate available audio devices.
   *  @return Vector of DeviceHandle objects, one per physical audio device.
   */
  virtual std::vector<DeviceHandlePtr> get_audio_devices();

  /** @brief Set the output device for the audio stream.
   *  @param device DeviceHandle representing an audio output device.
   *  @throws std::invalid_argument if device is null or not an output device.
   */
  void set_output_device(DeviceHandlePtr device);

  /** @brief Get the currently configured output device handle.
   *  @return DeviceHandlePtr, or nullptr if none has been set.
   */
  DeviceHandlePtr get_device_handle() const { return m_device_handle; }

  /** @brief Get the audio callback context shared with the data plane.
   *  @return Shared pointer to the AudioCallbackContext.
   */
  virtual std::shared_ptr<AudioCallbackContext> get_callback_context() const
  {
    return m_callback_context;
  }

protected:
  /** @brief Validates preconditions before starting the audio stream.
   *  @return true if all preconditions are met, false otherwise.
   */
  bool validate_start_preconditions() const;

  /** @brief Registers active tracks from TrackService for audio callbacks.
   *  @return true if tracks were successfully registered, false if no active tracks.
   */
  bool register_dataplanes();

  // Common state shared by all implementations
  std::shared_ptr<AudioCallbackContext> m_callback_context;
  DeviceHandlePtr m_device_handle;

private:
  adapter::AudioAdapter m_adapter;

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

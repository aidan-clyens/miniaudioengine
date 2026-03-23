#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__

#include <vector>
#include <string>
#include <optional>
#include <stdexcept>

#include "devicehandle.h"
#include "audiocontroller.h"
#include "midicontroller.h"

namespace miniaudioengine
{

/** @class DeviceManager
 *  @brief This class manages the system's audio and MIDI I/O devices.
 *  It is implemented as a singleton to provide a global point of access.
 */
class DeviceManager
{
public:
  static DeviceManager& instance()
  {
    static DeviceManager instance;
    return instance;
  }

  /** @brief Return a list of all available audio devices.
   *  @return A vector of DeviceHandle objects representing the available audio devices.
   */
  std::vector<DeviceHandlePtr> get_audio_devices() const;

  /** @brief Returns an audio device by its ID.
   *  @param id The ID of the audio device to get.
   *  @return The DeviceHandle for the specified audio device.
   *  @throws std::out_of_range if the device with the specified ID does not exist.
   */
  DeviceHandlePtr get_audio_device(const unsigned int id) const;

  /** @brief Return a list of all available MIDI devices.
   *  @return A vector of DeviceHandle objects representing the available MIDI devices.
   */
  std::vector<DeviceHandlePtr> get_midi_devices() const;

  /** @brief Returns a MIDI device by its ID.
   *  @param id The ID of the MIDI device to get.
   *  @return The DeviceHandle for the specified MIDI device.
   *  @throws std::out_of_range if the device with the specified ID does not exist.
   */
  DeviceHandlePtr get_midi_device(const unsigned int id) const;

  /** @brief Get the default audio input device, if available.
   *  @return DeviceHandlePtr for the default audio input, or nullptr if none.
   */
  DeviceHandlePtr get_default_audio_input_device();

  /** @brief Get the default audio output device, if available.
   *  @return DeviceHandlePtr for the default audio output, or nullptr if none.
   */
  DeviceHandlePtr get_default_audio_output_device();

  /** @brief Get the default MIDI input device, if available.
   *  @return DeviceHandlePtr for the default MIDI input, or nullptr if none.
   */
  DeviceHandlePtr get_default_midi_input_device();

  /** @brief Get the default MIDI output device, if available.
   *  @return DeviceHandlePtr for the default MIDI output, or nullptr if none.
   */
  DeviceHandlePtr get_default_midi_output_device();

  /** @brief Replace the audio controller. Intended for unit testing only.
   *  @param controller The controller to use in place of the default AudioController.
   */
  void set_audio_controller(std::shared_ptr<audio::AudioController> controller);

  /** @brief Replace the MIDI controller. Intended for unit testing only.
   *  @param controller The controller to use in place of the default MidiController.
   */
  void set_midi_controller(std::shared_ptr<midi::MidiController> controller);

private:
  DeviceManager():
    p_audio_controller(std::make_shared<audio::AudioController>()),
    p_midi_controller(std::make_shared<midi::MidiController>())
  {}
  ~DeviceManager() = default;

  DeviceManager(const DeviceManager&) = delete;
  DeviceManager& operator=(const DeviceManager&) = delete;

private:
  std::shared_ptr<audio::AudioController> p_audio_controller;
  std::shared_ptr<midi::MidiController> p_midi_controller;
};

} // namespace miniaudioengine

#endif  // __DEVICE_MANAGER_H__
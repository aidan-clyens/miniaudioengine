#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__

#include <vector>
#include <string>
#include <optional>
#include <stdexcept>

#include "device.h"
#include "audiodevice.h"
#include "mididevice.h"

namespace MinimalAudioEngine::Control
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
   *  @return A vector of AudioDevice objects representing the available audio devices.
   */
  std::vector<AudioDevice> get_audio_devices() const;

  /** @brief Returns an audio device by its ID.
   *  @param id The ID of the audio device to get.
   *  @return The AudioDevice object with the specified ID.
   *  @throws std::out_of_range if the device with the specified ID does not exist.
   */
  AudioDevice get_audio_device(const unsigned int id) const;

  /** @brief Return a list of all available MIDI devices.
   *  @return A vector of MidiDevice objects representing the available MIDI devices.
   */
  std::vector<MidiDevice> get_midi_devices() const;

  /** @brief Returns a MIDI device by its ID.
   *  @param id The ID of the MIDI device to get.
   *  @return The MidiDevice object with the specified ID.
   *  @throws std::out_of_range if the device with the specified ID does not exist.
   */
  MidiDevice get_midi_device(const unsigned int id) const;

  /** @brief Get the default audio input device, if available.
   *  @return An optional AudioDevice object representing the default audio input device.
   */
  std::optional<AudioDevice> get_default_audio_input_device();

  /** @brief Get the default audio output device, if available.
   *  @return An optional AudioDevice object representing the default audio output device.
   */
  std::optional<AudioDevice> get_default_audio_output_device();
  
  /** @brief Get the default MIDI input device, if available.
   *  @return An optional MidiDevice object representing the default MIDI input device.
   */
  std::optional<MidiDevice> get_default_midi_input_device();
  
  /** @brief Get the default MIDI output device, if available.
   *  @return An optional MidiDevice object representing the default MIDI output device.
   */
  std::optional<MidiDevice> get_default_midi_output_device();

private:
  DeviceManager() = default;
  ~DeviceManager() = default;

  DeviceManager(const DeviceManager&) = delete;
  DeviceManager& operator=(const DeviceManager&) = delete;
};

} // namespace MinimalAudioEngine::Control

#endif  // __DEVICE_MANAGER_H__
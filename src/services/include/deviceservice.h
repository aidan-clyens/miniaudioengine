#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <stdexcept>

namespace miniaudioengine
{

// Forward declarations
namespace adapters { class AudioAdapter; }
namespace adapters { class MidiAdapter; }

class Device;

using DevicePtr = std::shared_ptr<Device>;
using DeviceList = std::vector<DevicePtr>;

/** @class DeviceService
 *  @brief This class manages the system's audio and MIDI I/O devices.
 *  It is implemented as a singleton to provide a global point of access.
 */
class DeviceService
{
public:
  DeviceService(std::shared_ptr<adapters::AudioAdapter> audio_adapter, std::shared_ptr<adapters::MidiAdapter> midi_adapter);
  ~DeviceService();

  /** @brief Return a list of all available audio devices.
   *  @return A vector of Device objects representing the available audio devices.
   */
  DeviceList get_audio_devices() const;

  /** @brief Returns an audio device by its ID.
   *  @param id The ID of the audio device to get.
   *  @return The Device for the specified audio device.
   *  @throws std::out_of_range if the device with the specified ID does not exist.
   */
  DevicePtr get_audio_device(const unsigned int id) const;

  /** @brief Return a list of all available MIDI devices.
   *  @return A vector of Device objects representing the available MIDI devices.
   */
  DeviceList get_midi_devices() const;

  /** @brief Returns a MIDI device by its ID.
   *  @param id The ID of the MIDI device to get.
   *  @return The Device for the specified MIDI device.
   *  @throws std::out_of_range if the device with the specified ID does not exist.
   */
  DevicePtr get_midi_device(const unsigned int id) const;

  /** @brief Get the default audio input device, if available.
   *  @return DevicePtr for the default audio input, or nullptr if none.
   */
  DevicePtr get_default_audio_input_device();

  /** @brief Get the default audio output device, if available.
   *  @return DevicePtr for the default audio output, or nullptr if none.
   */
  DevicePtr get_default_audio_output_device();

  /** @brief Get the default MIDI input device, if available.
   *  @return DevicePtr for the default MIDI input, or nullptr if none.
   */
  DevicePtr get_default_midi_input_device();

  /** @brief Get the default MIDI output device, if available.
   *  @return DevicePtr for the default MIDI output, or nullptr if none.
   */
  DevicePtr get_default_midi_output_device();

private:
  std::shared_ptr<adapters::AudioAdapter> p_audio_adapter;
  std::shared_ptr<adapters::MidiAdapter> p_midi_adapter;
};

} // namespace miniaudioengine

#endif  // __DEVICE_MANAGER_H__
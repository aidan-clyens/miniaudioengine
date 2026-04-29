#ifndef __DEVICE_HANDLE_H__
#define __DEVICE_HANDLE_H__

#include <memory>
#include <string>
#include <vector>

namespace miniaudioengine
{

/** @class Device
 *  @brief Unified PImpl handle for audio and MIDI devices.
 *  Hides hardware backend (RtAudio/RtMidi) from public headers.
 */
class Device
{
public:
  /** @enum eDeviceType
   *  @brief Discriminates between audio and MIDI device handles.
   */
  enum class eDeviceType
  {
    Audio,
    Midi
  };

  ~Device();

  Device(const Device&) = delete;
  Device& operator=(const Device&) = delete;

  // -------------------------------------------------------------------------
  // Common accessors
  // -------------------------------------------------------------------------

  /** @brief Returns the hardware device ID. */
  unsigned int get_id() const;

  /** @brief Returns the human-readable device name. */
  const std::string& get_name() const;

  /** @brief Returns whether this is an Audio or Midi device handle. */
  eDeviceType get_device_type() const;

  /** @brief Returns true if the device has at least one input channel. */
  bool is_input() const;

  /** @brief Returns true if the device has at least one output channel. */
  bool is_output() const;

  /** @brief Returns true if this is the system default input device. */
  bool is_default_input() const;

  /** @brief Returns true if this is the system default output device. */
  bool is_default_output() const;

  /** @brief Returns a human-readable description of the device. */
  std::string to_string() const;

  bool operator==(const Device& other) const;

  // -------------------------------------------------------------------------
  // Audio-only accessors (valid only when get_device_type() == eDeviceType::Audio)
  // -------------------------------------------------------------------------

  /** @brief Returns the number of output channels. Returns 0 for MIDI devices. */
  unsigned int get_output_channels() const;

  /** @brief Returns the number of input channels. Returns 0 for MIDI devices. */
  unsigned int get_input_channels() const;

  /** @brief Returns the number of duplex channels. Returns 0 for MIDI devices. */
  unsigned int get_duplex_channels() const;

  /** @brief Returns supported sample rates. Returns empty vector for MIDI devices. */
  const std::vector<unsigned int>& get_sample_rates() const;

  /** @brief Returns the preferred sample rate. Returns 0 for MIDI devices. */
  unsigned int get_preferred_sample_rate() const;

private:
  struct Impl;
  explicit Device(std::unique_ptr<Impl> impl);
  std::unique_ptr<Impl> p_impl;

  // Internal factory methods — called only within the library implementation
  friend class DeviceHandleFactory;
};

using DeviceHandlePtr = std::shared_ptr<Device>;

} // namespace miniaudioengine

#endif // __DEVICE_HANDLE_H__

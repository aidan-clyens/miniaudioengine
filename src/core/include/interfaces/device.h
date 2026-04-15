#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <string>
#include <memory>

namespace miniaudioengine::core
{

/** @class IDevice
 *  @brief Base structure for audio and MIDI devices
 */
class IDevice
{
public:
  IDevice() = default;
  virtual ~IDevice() = default;

  IDevice(const IDevice &) = default;
  IDevice &operator=(const IDevice &) = default;

  unsigned int id;
  std::string name;
  bool is_default_output;
  bool is_default_input;

  virtual bool is_input() const = 0;
  virtual bool is_output() const = 0;

  std::string to_string() const
  {
    return "Device(ID=" + std::to_string(id) +
            ", Name=" + name +
            ", DefaultOutput=" + (is_default_output ? "Yes" : "No") +
            ", DefaultInput=" + (is_default_input ? "Yes" : "No") + ")";
  }

  bool operator==(const IDevice &other) const
  {
    return id == other.id && name == other.name &&
            is_default_output == other.is_default_output &&
            is_default_input == other.is_default_input;
  }
};

using IDevicePtr = std::shared_ptr<IDevice>;

/** @class AudioDevice
 *  @brief Represents an audio device with specific properties.
 */
class IAudioDevice : public IDevice
{
public:
  IAudioDevice() = default;
  ~IAudioDevice() override = default;

  IAudioDevice(const IAudioDevice &) = default;
  IAudioDevice &operator=(const IAudioDevice &) = default;

  unsigned int output_channels;
  unsigned int input_channels;
  unsigned int duplex_channels;
  std::vector<unsigned int> sample_rates;
  unsigned int preferred_sample_rate;

  bool is_input() const override
  {
    return input_channels > 0 || duplex_channels > 0;
  }

  bool is_output() const override
  {
    return output_channels > 0 || duplex_channels > 0;
  }
};

using IAudioDevicePtr = std::shared_ptr<IAudioDevice>;

} // namespace miniaudioengine::core

#endif // __DEVICE_H__
#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__

#include <vector>
#include <string>

namespace Devices
{

struct Device
{
  Device() = default;
  ~Device() = default;

  unsigned int id;
  std::string name;
  bool is_default_output;
  bool is_default_input;
};

struct AudioDevice : public Device
{
  AudioDevice() = default;
  ~AudioDevice() = default;

  unsigned int output_channels;
  unsigned int input_channels;
  unsigned int duplex_channels;
  std::vector<unsigned int> sample_rates;
  unsigned int preferred_sample_rate;
};

struct MidiDevice : public Device
{
  MidiDevice() = default;
  ~MidiDevice() = default;
};

class DeviceManager
{
public:
  static DeviceManager& instance()
  {
    static DeviceManager instance;
    return instance;
  }

  std::vector<AudioDevice> get_audio_devices() const;
  AudioDevice get_audio_device(const unsigned int id) const;

  std::vector<MidiDevice> get_midi_devices() const;
  MidiDevice get_midi_device(const unsigned int id) const;

private:
  DeviceManager() = default;
  ~DeviceManager() = default;

  DeviceManager(const DeviceManager&) = delete;
  DeviceManager& operator=(const DeviceManager&) = delete;
};

} // namespace Devices

#endif  // __DEVICE_MANAGER_H__
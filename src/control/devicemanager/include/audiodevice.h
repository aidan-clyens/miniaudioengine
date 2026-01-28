#ifndef __AUDIO_DEVICE_H__
#define __AUDIO_DEVICE_H__

#include "device.h"

#include <vector>
#include <string>
#include <memory>
#include <rtaudio/RtAudio.h>

namespace miniaudioengine::control
{

/** @class AudioDevice
 *  @brief Audio device
 */
class AudioDevice : public core::IDevice
{
public:
  AudioDevice() = default;
  AudioDevice(RtAudio::DeviceInfo info): core::IDevice()
  {
    id = info.ID;
    name = info.name;
    is_default_output = info.isDefaultOutput;
    is_default_input = info.isDefaultInput;
    output_channels = info.outputChannels;
    input_channels = info.inputChannels;
    duplex_channels = info.duplexChannels;
    sample_rates = info.sampleRates;
    preferred_sample_rate = info.preferredSampleRate;
  }

  virtual ~AudioDevice() = default;

  AudioDevice(const AudioDevice&) = default;
  AudioDevice& operator=(const AudioDevice&) = default;

  unsigned int output_channels;
  unsigned int input_channels;
  unsigned int duplex_channels;
  std::vector<unsigned int> sample_rates;
  unsigned int preferred_sample_rate;

  bool is_input() const override
  {
    return input_channels > 0;
  }

  bool is_output() const override
  {
    return output_channels > 0;
  }

  std::string to_string() const
  {
    return "AudioDevice(ID=" + std::to_string(id) +
           ", Name=" + name +
           ", DefaultOutput=" + (is_default_output ? "Yes" : "No") +
           ", DefaultInput=" + (is_default_input ? "Yes" : "No") +
           ", InputChannels=" + std::to_string(input_channels) +
           ", OutputChannels=" + std::to_string(output_channels) +
           ", PreferredSampleRate=" + std::to_string(preferred_sample_rate) + ")";
  }

  bool operator==(const AudioDevice& other) const
  {
    return id == other.id && name == other.name &&
           is_default_output == other.is_default_output &&
           is_default_input == other.is_default_input &&
           input_channels == other.input_channels &&
           output_channels == other.output_channels &&
           duplex_channels == other.duplex_channels &&
           sample_rates == other.sample_rates &&
           preferred_sample_rate == other.preferred_sample_rate;
  }
};

using AudioDevicePtr = std::shared_ptr<AudioDevice>;

} // namespace miniaudioengine::control

#endif // __AUDIO_DEVICE_H__
#include "device.h"

#include <string>
#include <vector>
#include <memory>

namespace miniaudioengine
{

// =============================================================================
// Device::Impl — defined here so RtAudio headers stay out of the public API
// =============================================================================

struct Device::Impl
{
  Device::eDeviceType device_type;

  // Common fields
  unsigned int id{0};
  std::string  name;
  bool is_default_input{false};
  bool is_default_output{false};

  // Audio-only fields (zeroed/empty for MIDI)
  unsigned int output_channels{0};
  unsigned int input_channels{0};
  unsigned int duplex_channels{0};
  std::vector<unsigned int> sample_rates;
  unsigned int preferred_sample_rate{0};
};

// =============================================================================
// Device — member implementations
// =============================================================================

Device::Device(std::unique_ptr<Impl> impl)
  : p_impl(std::move(impl)) {}

Device::~Device() = default;

unsigned int Device::get_id() const { return p_impl->id; }
const std::string& Device::get_name() const { return p_impl->name; }
Device::eDeviceType Device::get_device_type() const { return p_impl->device_type; }
bool Device::is_default_input() const { return p_impl->is_default_input; }
bool Device::is_default_output() const { return p_impl->is_default_output; }
unsigned int Device::get_output_channels() const { return p_impl->output_channels; }
unsigned int Device::get_input_channels() const { return p_impl->input_channels; }
unsigned int Device::get_duplex_channels() const { return p_impl->duplex_channels; }
const std::vector<unsigned int>& Device::get_sample_rates() const { return p_impl->sample_rates; }
unsigned int Device::get_preferred_sample_rate() const { return p_impl->preferred_sample_rate; }

bool Device::is_input() const
{
  if (p_impl->device_type == eDeviceType::Audio)
    return p_impl->input_channels > 0 || p_impl->duplex_channels > 0;
  return p_impl->is_default_input;
}

bool Device::is_output() const
{
  if (p_impl->device_type == eDeviceType::Audio)
    return p_impl->output_channels > 0 || p_impl->duplex_channels > 0;
  return p_impl->is_default_output;
}

std::string Device::to_string() const
{
  if (p_impl->device_type == eDeviceType::Audio)
  {
    return "Device(Type=Audio"
           ", ID=" + std::to_string(p_impl->id) +
           ", Name=" + p_impl->name +
           ", DefaultOutput=" + (p_impl->is_default_output ? "Yes" : "No") +
           ", DefaultInput=" + (p_impl->is_default_input ? "Yes" : "No") +
           ", InputChannels=" + std::to_string(p_impl->input_channels) +
           ", OutputChannels=" + std::to_string(p_impl->output_channels) +
           ", PreferredSampleRate=" + std::to_string(p_impl->preferred_sample_rate) + ")";
  }
  return "Device(Type=Midi"
         ", ID=" + std::to_string(p_impl->id) +
         ", Name=" + p_impl->name +
         ", DefaultOutput=" + (p_impl->is_default_output ? "Yes" : "No") +
         ", DefaultInput=" + (p_impl->is_default_input ? "Yes" : "No") + ")";
}

bool Device::operator==(const Device& other) const
{
  return p_impl->id == other.p_impl->id &&
         p_impl->name == other.p_impl->name &&
         p_impl->device_type == other.p_impl->device_type &&
         p_impl->is_default_input == other.p_impl->is_default_input &&
         p_impl->is_default_output == other.p_impl->is_default_output;
}

// =============================================================================
// DeviceHandleFactory
// =============================================================================

DeviceHandlePtr DeviceHandleFactory::make_audio(unsigned int id,
                                                const std::string& name,
                                                bool is_default_input,
                                                bool is_default_output,
                                                unsigned int output_channels,
                                                unsigned int input_channels,
                                                unsigned int duplex_channels,
                                                unsigned int preferred_sample_rate,
                                                const std::vector<unsigned int>& sample_rates)
{
  auto impl = std::make_unique<Device::Impl>();
  impl->device_type           = Device::eDeviceType::Audio;
  impl->id                    = id;
  impl->name                  = name;
  impl->is_default_input      = is_default_input;
  impl->is_default_output     = is_default_output;
  impl->output_channels       = output_channels;
  impl->input_channels        = input_channels;
  impl->duplex_channels       = duplex_channels;
  impl->sample_rates          = sample_rates;
  impl->preferred_sample_rate = preferred_sample_rate;
  return DeviceHandlePtr(new Device(std::move(impl)));
}

DeviceHandlePtr DeviceHandleFactory::make_midi(unsigned int id,
                                               const std::string& name,
                                               bool is_input,
                                               bool is_output)
{
  auto impl = std::make_unique<Device::Impl>();
  impl->device_type       = Device::eDeviceType::Midi;
  impl->id                = id;
  impl->name              = name;
  impl->is_default_input  = is_input;
  impl->is_default_output = is_output;
  return DeviceHandlePtr(new Device(std::move(impl)));
}

} // namespace miniaudioengine

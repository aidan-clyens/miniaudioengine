#include "deviceservice.h"

// entities
#include "device.h"

// adapters
#include "audioadapter.h"
#include "midiadapter.h"


using namespace miniaudioengine;
using namespace miniaudioengine::adapters;


DeviceService::DeviceService(std::shared_ptr<AudioAdapter> audio_adapter, std::shared_ptr<MidiAdapter> midi_adapter)
    : p_audio_adapter(audio_adapter), p_midi_adapter(midi_adapter) {}

DeviceService::~DeviceService() = default;

DevicePtr DeviceService::get_audio_device(const unsigned int id) const
{
  for (const auto& device : get_audio_devices())
  {
    if (device->get_id() == id)
      return device;
  }
  throw std::out_of_range("Audio device with ID " + std::to_string(id) + " does not exist");
}

DeviceList DeviceService::get_audio_devices() const
{
  return p_audio_adapter->get_devices();
}

DeviceList DeviceService::get_midi_devices() const
{
  DeviceList devices;
  // TODO - populate devices vector with actual MIDI devices from the system
  return devices;
}

DevicePtr DeviceService::get_midi_device(const unsigned int id) const
{
  for (const auto& device : get_midi_devices())
  {
    if (device->get_id() == id)
      return device;
  }
  throw std::out_of_range("MIDI device with ID " + std::to_string(id) + " does not exist");
}

DevicePtr DeviceService::get_default_audio_input_device()
{
  for (const auto& dev : get_audio_devices())
  {
    if (dev->is_default_input())
      return dev;
  }
  return nullptr;
}

DevicePtr DeviceService::get_default_audio_output_device()
{
  for (const auto& dev : get_audio_devices())
  {
    if (dev->is_default_output())
      return dev;
  }
  return nullptr;
}

DevicePtr DeviceService::get_default_midi_input_device()
{
  for (const auto& dev : get_midi_devices())
  {
    if (dev->is_default_input())
      return dev;
  }
  return nullptr;
}

DevicePtr DeviceService::get_default_midi_output_device()
{
  for (const auto& dev : get_midi_devices())
  {
    if (dev->is_default_output())
      return dev;
  }
  return nullptr;
}
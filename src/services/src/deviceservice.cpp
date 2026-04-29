#include "deviceservice.h"

using namespace miniaudioengine;

DeviceHandlePtr DeviceService::get_audio_device(const unsigned int id) const
{
  for (const auto& device : get_audio_devices())
  {
    if (device->get_id() == id)
      return device;
  }
  throw std::out_of_range("Audio device with ID " + std::to_string(id) + " does not exist");
}

std::vector<DeviceHandlePtr> DeviceService::get_midi_devices() const
{
  std::vector<DeviceHandlePtr> devices;
  // for (const auto& port : p_midi_controller->get_ports())
  // {
  //   devices.push_back(DeviceHandleFactory::make_midi(
  //     port.port_number,
  //     port.port_name,
  //     /*is_input=*/true,
  //     /*is_output=*/false));
  // }
  return devices;
}

DeviceHandlePtr DeviceService::get_midi_device(const unsigned int id) const
{
  for (const auto& device : get_midi_devices())
  {
    if (device->get_id() == id)
      return device;
  }
  throw std::out_of_range("MIDI device with ID " + std::to_string(id) + " does not exist");
}

DeviceHandlePtr DeviceService::get_default_audio_input_device()
{
  for (const auto& dev : get_audio_devices())
  {
    if (dev->is_default_input())
      return dev;
  }
  return nullptr;
}

DeviceHandlePtr DeviceService::get_default_audio_output_device()
{
  for (const auto& dev : get_audio_devices())
  {
    if (dev->is_default_output())
      return dev;
  }
  return nullptr;
}

DeviceHandlePtr DeviceService::get_default_midi_input_device()
{
  for (const auto& dev : get_midi_devices())
  {
    if (dev->is_default_input())
      return dev;
  }
  return nullptr;
}

DeviceHandlePtr DeviceService::get_default_midi_output_device()
{
  for (const auto& dev : get_midi_devices())
  {
    if (dev->is_default_output())
      return dev;
  }
  return nullptr;
}
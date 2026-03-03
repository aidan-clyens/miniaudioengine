#include "devicemanager.h"

using namespace miniaudioengine::core;
using namespace miniaudioengine;
using namespace miniaudioengine::audio;
using namespace miniaudioengine::midi;

std::vector<IAudioDevicePtr> DeviceManager::get_audio_devices() const
{
  return p_audio_controller->get_audio_devices();
}

IAudioDevicePtr DeviceManager::get_audio_device(const unsigned int id) const
{
  auto devices = get_audio_devices();
  for (const auto &device : devices)
  {
    if (device->id == id)
    {
      return std::dynamic_pointer_cast<AudioDevice>(device);
    }
  }

  throw std::out_of_range("Audio device with ID " + std::to_string(id) + " does not exist");
}

std::vector<IDevicePtr> DeviceManager::get_midi_devices() const
{
  std::vector<IDevicePtr> devices;
  auto midi_devices = p_midi_controller->get_ports();

  for (const auto &port : midi_devices)
  {
    auto device = std::make_shared<MidiDevice>();
    device->id = port.port_number;
    device->name = port.port_name;
    devices.push_back(device);
  }

  return devices;
}

IDevicePtr DeviceManager::get_midi_device(const unsigned int id) const
{
  auto devices = get_midi_devices();
  for (const auto &device : devices)
  {
    if (device->id == id)
    {
      return device;
    }
  }

  throw std::out_of_range("MIDI device with ID " + std::to_string(id) + " does not exist");
}

IDevicePtr DeviceManager::get_default_audio_input_device()
{
  auto devices = get_audio_devices();
  for (const auto& dev : devices)
  {
    if (dev->is_default_input)
    {
      return dev;
    }
  }

  return nullptr;
}

IDevicePtr DeviceManager::get_default_audio_output_device()
{
  auto devices = get_audio_devices();
  for (const auto& dev : devices)
  {
    if (dev->is_default_output)
    {
      return dev;
    }
  }

  return nullptr;
}

IDevicePtr DeviceManager::get_default_midi_input_device()
{
  auto devices = get_midi_devices();
  for (const auto& dev : devices)
  {
    if (dev->is_default_input)
    {
      return dev;
    }
  }

  return nullptr;
}

IDevicePtr DeviceManager::get_default_midi_output_device() {
  auto devices = get_midi_devices();
  for (const auto& dev : devices)
  {
    if (dev->is_default_output)
    {
      return dev;
    }
  }


  return nullptr;
}
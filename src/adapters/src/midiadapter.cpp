#include "midiadapter.h"

using namespace miniaudioengine;
using namespace miniaudioengine::adapters;

MidiAdapter::MidiAdapter()
{
  std::vector<RtMidi::Api> apis;
  RtMidi::getCompiledApi(apis);
  LOG_DEBUG("MidiAdapter: Compiled RtMidi APIs = ", apis.size());
  for (const auto api : apis)
  {
    LOG_DEBUG("MidiAdapter: Compiled API - ", RtMidi::getApiDisplayName(api));
  }

  try
  {
    p_rtmidi_in = std::make_unique<RtMidiIn>();
  }
  catch (RtMidiError &error)
  {
    LOG_ERROR("MidiAdapter: Failed to initialize RtMidiIn!");
    throw std::runtime_error("MidiAdapter: Failed to initialize RtMidiIn!");
  }
}

unsigned int MidiAdapter::get_device_count()
{
  return p_rtmidi_in->getPortCount();
}

std::vector<DevicePtr> MidiAdapter::get_devices()
{
  std::vector<DevicePtr> devices;

  // Get the number of available MIDI input ports
  unsigned int port_count = p_rtmidi_in->getPortCount();
  LOG_DEBUG("MidiAdapter: Number of MIDI input ports: ", port_count);

  // List all available MIDI input ports
  for (unsigned int i = 0; i < port_count; ++i)
  {
    try
    {
      std::string port_name = p_rtmidi_in->getPortName(i);
      devices.push_back(make_device_handle(i, port_name));
    }
    catch (const RtMidiError &error)
    {
      LOG_ERROR("MidiAdapter: Error getting port name: ", error.getMessage());
    }
  }

  return devices;
}

bool MidiAdapter::open_input_port(DevicePtr device, void *callback_context)
{
  if (device->get_device_type() != Device::eDeviceType::Midi)
  {
    LOG_ERROR("MidiAdapter: Cannot open MIDI port. Device is not a MIDI device!");
    throw std::runtime_error("MidiAdapter: Cannot open MIDI port. Device is not a MIDI device!");
  }

  LOG_DEBUG("MidiAdapter: Opening MIDI port ", device->to_string());

  p_rtmidi_in->setCallback(&MidiCallbackHandler::midi_callback, callback_context);
  p_rtmidi_in->ignoreTypes(false, true, true);

  // Set up the MIDI input port
  try
  {
    p_rtmidi_in->openPort(device->get_port_number());
  }
  catch (const RtMidiError &error)
  {
    LOG_ERROR("MidiAdapter: Failed to open MIDI input port: ", error.getMessage());
    return false;
  }

  LOG_DEBUG("MidiAdapter: Opened MIDI input port ", device->to_string());
  return true;
}

bool MidiAdapter::close_input_port()
{
  try
  {
    p_rtmidi_in->closePort();
    LOG_DEBUG("MidiAdapter: Closed MIDI input port.");
  }
  catch (const RtMidiError &error)
  {
    LOG_ERROR("MidiAdapter: Error closing MIDI input port: ", error.getMessage());
    return false;
  }
  return true;
}

bool MidiAdapter::is_port_open()
{
  return p_rtmidi_in->isPortOpen();
}

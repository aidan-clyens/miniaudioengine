#include "midiadapter.h"

using namespace miniaudioengine::adapters;

std::vector<MidiPort> MidiAdapter::get_ports()
{
  std::vector<MidiPort> ports;

  // Get the number of available MIDI input ports
  unsigned int port_count = m_rtmidi_in.getPortCount();
  LOG_DEBUG("MidiAdapter: Number of MIDI input ports: ", port_count);

  // List all available MIDI input ports
  for (unsigned int i = 0; i < port_count; ++i)
  {
    try
    {
      std::string port_name = m_rtmidi_in.getPortName(i);
      ports.push_back({i, port_name});
    }
    catch (const RtMidiError &error)
    {
      LOG_ERROR("MidiAdapter: Error getting port name: ", error.getMessage());
    }
  }

  return ports;
}

bool MidiAdapter::open_input_port(unsigned int port_number, void *callback_context)
{
  m_rtmidi_in.setCallback(&MidiCallbackHandler::midi_callback, callback_context);
  m_rtmidi_in.ignoreTypes(false, true, true);

  // Set up the MIDI input port
  try
  {
    m_rtmidi_in.openPort(port_number);
  }
  catch (const RtMidiError &error)
  {
    LOG_ERROR("MidiAdapter: Failed to open MIDI input port: ", error.getMessage());
    return false;
  }

  LOG_DEBUG("MidiAdapter: Opened MIDI input port (ID=", port_number, ", Name=", m_rtmidi_in.getPortName(port_number), ")");
  return true;
}

bool MidiAdapter::close_input_port()
{
  try
  {
    m_rtmidi_in.closePort();
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
  return m_rtmidi_in.isPortOpen();
}

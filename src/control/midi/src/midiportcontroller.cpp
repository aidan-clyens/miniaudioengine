#include "midiportcontroller.h"

#include "mididataplane.h"
#include "logger.h"

using namespace miniaudioengine::control;
using namespace miniaudioengine::data;

/** @brief Lists all available MIDI input ports.
 *  This function retrieves and prints the names of all available MIDI input ports.
 *
 *  @return A vector of MidiPort objects representing the available MIDI ports.
 */
std::vector<MidiPort> MidiPortController::get_ports()
{
  std::vector<MidiPort> ports;

  // Get the number of available MIDI input ports
  unsigned int port_count = m_rtmidi_in.getPortCount();
  LOG_DEBUG("MidiPortController: Number of MIDI input ports: ", port_count);

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
      LOG_ERROR("MidiPortController: Error getting port name: ", error.getMessage());
    }
  }

  return ports;
}

/** @brief Opens a MIDI input device port.
 *  @param port_number The MIDI device port number to open (default is 0).
 *  @throws std::out_of_range if the port number is invalid.
 *  @throws std::runtime_error if the port cannot be opened.
 */
void MidiPortController::open_input_port(unsigned int port_number)
{
  if (port_number >= m_rtmidi_in.getPortCount())
  {
    LOG_ERROR("MidiPortController: Invalid MIDI port number: ", port_number);
    throw std::out_of_range("Invalid MIDI port number: " + std::to_string(port_number));
  }

  // Check if context is initialized
  if (!m_callback_context)
  {
    LOG_ERROR("MidiPortController: MIDI callback context is not initialized.");
    throw std::runtime_error("MIDI callback context is not initialized.");
  }

  // Check if port is already open
  if (m_rtmidi_in.isPortOpen())
  {
    LOG_WARNING("MidiPortController: MIDI input port is already open. Closing existing port.");
    close_input_port();
  }

  // Set the callback function to handle incoming MIDI messages
  m_callback_context->active_tracks.clear();
  for (const auto &dataplane : get_registered_dataplanes())
  {
    auto midi_dataplane = std::dynamic_pointer_cast<MidiDataPlane>(dataplane);
    if (midi_dataplane)
    {
      m_callback_context->active_tracks.push_back(midi_dataplane);
    }
  }

  if (m_callback_context->active_tracks.empty())
  {
    LOG_WARNING("MidiPortController: No active MIDI dataplanes registered.");
    return;
  }

  m_rtmidi_in.setCallback(&MidiCallbackHandler::midi_callback, m_callback_context.get());
  m_rtmidi_in.ignoreTypes(false, true, true);

  // Set up the MIDI input port
  try
  {
    m_rtmidi_in.openPort(port_number);
  }
  catch (const RtMidiError &error)
  {
    LOG_ERROR("MidiPortController: Failed to open MIDI input port: ", error.getMessage());
    return;
  }

  LOG_DEBUG("MidiPortController: MIDI input port opened successfully.");
}

/** @brief Closes the currently opened MIDI input port.
 */
void MidiPortController::close_input_port()
{
  if (!m_rtmidi_in.isPortOpen())
  {
    return;
  }

  try
  {
    m_rtmidi_in.closePort();
    LOG_DEBUG("MidiPortController: MIDI input port closed successfully.");
  }
  catch (const RtMidiError &error)
  {
    LOG_ERROR("MidiPortController: Error closing MIDI input port: ", error.getMessage());
  }
}

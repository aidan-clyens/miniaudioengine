#include "midiportcontroller.h"

#include "trackmanager.h"
#include "logger.h"

using namespace MinimalAudioEngine::Control;
using namespace MinimalAudioEngine::Data;

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
  LOG_INFO("Number of MIDI input ports: ", port_count);

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
      LOG_ERROR("Error getting port name: ", error.getMessage());
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
    LOG_ERROR("Invalid MIDI port number: ", port_number);
    throw std::out_of_range("Invalid MIDI port number: " + std::to_string(port_number));
  }

  // Check if context is initialized
  if (!m_callback_context)
  {
    LOG_ERROR("MIDI callback context is not initialized.");
    throw std::runtime_error("MIDI callback context is not initialized.");
  }

  // Check if port is already open
  if (m_rtmidi_in.isPortOpen())
  {
    LOG_WARNING("MIDI input port is already open. Closing existing port.");
    close_input_port();
  }

  // Set up the MIDI input port
  try
  {
    m_rtmidi_in.openPort(port_number);
  }
  catch (const RtMidiError &error)
  {
    LOG_ERROR("Failed to open MIDI input port: ", error.getMessage());
    return;
  }

  // Set the callback function to handle incoming MIDI messages
  m_callback_context->active_tracks.clear();
  m_callback_context->active_tracks = TrackManager::instance().get_track_midi_dataplanes();

  m_rtmidi_in.setCallback(&MidiCallbackHandler::midi_callback, m_callback_context.get());
  m_rtmidi_in.ignoreTypes(false, true, true);

  LOG_INFO("MIDI input port opened successfully.");
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
    LOG_INFO("MIDI input port closed successfully.");
  }
  catch (const RtMidiError &error)
  {
    LOG_ERROR("Error closing MIDI input port: ", error.getMessage());
  }
}

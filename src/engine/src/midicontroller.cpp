#include "midicontroller.h"

#include "mididataplane.h"
#include "logger.h"

using namespace miniaudioengine::midi;
using namespace miniaudioengine::core;

/** @brief Opens a MIDI input device port.
 *  @param port_number The MIDI device port number to open (default is 0).
 *  @throws std::out_of_range if the port number is invalid.
 *  @throws std::runtime_error if the port cannot be opened.
 */
void MidiController::open_input_port(unsigned int port_number)
{
  if (port_number >= m_midi_adapter.get_port_count())
  {
    LOG_ERROR("MidiController: Invalid MIDI port number: ", port_number);
    throw std::out_of_range("Invalid MIDI port number: " + std::to_string(port_number));
  }

  // Check if context is initialized
  if (!m_callback_context)
  {
    LOG_ERROR("MidiController: MIDI callback context is not initialized.");
    throw std::runtime_error("MIDI callback context is not initialized.");
  }

  // Check if port is already open
  if (m_midi_adapter.is_port_open())
  {
    LOG_WARNING("MidiController: MIDI input port is already open. Closing existing port.");
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
    LOG_WARNING("MidiController: No active MIDI dataplanes registered.");
    return;
  }

  if (!m_midi_adapter.open_input_port(port_number, m_callback_context.get()))
  {
    LOG_ERROR("MidiController: Failed to open MIDI input port number: ", port_number);
    throw std::runtime_error("Failed to open MIDI input port number: " + std::to_string(port_number));
  }
}

/** @brief Closes the currently opened MIDI input port.
 */
void MidiController::close_input_port()
{
  if (!m_midi_adapter.is_port_open())
  {
    return;
  }

  if (!m_midi_adapter.close_input_port())
  {
    LOG_ERROR("MidiController: Failed to close MIDI input port.");
    throw std::runtime_error("Failed to close MIDI input port.");
  }
}

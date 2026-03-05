#ifndef __MIDI_CONTROLLER_H__
#define __MIDI_CONTROLLER_H__

#include "midicontroller_interface.h"
#include "midicallbackhandler.h"
#include "miditypes.h"

#include <rtmidi/RtMidi.h>

namespace miniaudioengine::midi
{

/** @class MidiController
 *  @brief This class manages the MIDI hardware interfaces.
 *  It is responsible for initializing, configuring, and controlling MIDI inputs.
 *  The MidiController provides methods to open and close MIDI input ports.
 *  @note This class is part of the control plane. Operations are synchronous and called from the main thread.
 */
class MidiController : public IMidiController
{
public:
  explicit MidiController() = default;

  ~MidiController() override
  {
    close_input_port();
  }

  /** @brief Gets the list of available MIDI input ports.
   *  @return A vector of MidiPort structures representing the available MIDI ports.
   */
  std::vector<MidiPort> get_ports() override;

  /** @brief Opens a MIDI input device port.
   *  @param port_number The MIDI device port number to open (default is 0).
   *  @throws std::out_of_range if the port number is invalid.
   *  @throws std::runtime_error if the port cannot be opened.
   */
  void open_input_port(unsigned int port_number = 0) override;

  /** @brief Closes the currently opened MIDI input device port.
   */
  void close_input_port() override;

private:
  RtMidiIn m_rtmidi_in;

  bool _start() override { throw std::runtime_error("MidiController start/stop operations not implemented."); }
  bool _stop() override { throw std::runtime_error("MidiController start/stop operations not implemented."); }
};

using MidiControllerPtr = std::shared_ptr<MidiController>;

} // namespace miniaudioengine::midi

#endif // __MIDI_CONTROLLER_H__

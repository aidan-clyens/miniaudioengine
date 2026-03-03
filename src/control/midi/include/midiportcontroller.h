#ifndef __MIDI_PORT_CONTROLLER_H__
#define __MIDI_PORT_CONTROLLER_H__

#include "midicontroller.h"
#include "midicallbackhandler.h"
#include "miditypes.h"

#include <rtmidi/RtMidi.h>

namespace miniaudioengine::midi
{

class MidiPortController : public IMidiController
{
public:
  explicit MidiPortController() = default;

  ~MidiPortController() override
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

  bool _start() override { throw std::runtime_error("MidiPortController start/stop operations not implemented."); }
  bool _stop() override { throw std::runtime_error("MidiPortController start/stop operations not implemented."); }
};

using MidiPortControllerPtr = std::shared_ptr<MidiPortController>;

} // namespace miniaudioengine::midi

#endif // __MIDI_PORT_CONTROLLER_H__
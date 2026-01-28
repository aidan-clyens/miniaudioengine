#ifndef __MIDI_PORT_CONTROLLER_H__
#define __MIDI_PORT_CONTROLLER_H__

#include "midicontroller.h"
#include "midicallbackhandler.h"
#include "miditypes.h"

#include <rtmidi/RtMidi.h>

namespace miniaudioengine::control
{

class MidiPortController : public IMidiController
{
public:
  explicit MidiPortController():
    m_callback_context(std::make_shared<data::MidiCallbackContext>())
  {}

  virtual ~MidiPortController()
  {
    close_input_port();
  }

  /** @brief Gets the list of available MIDI input ports.
   *  @return A vector of MidiPort structures representing the available MIDI ports.
   */
  std::vector<MidiPort> get_ports();

  /** @brief Opens a MIDI input device port.
   *  @param port_number The MIDI device port number to open (default is 0).
   *  @throws std::out_of_range if the port number is invalid.
   *  @throws std::runtime_error if the port cannot be opened.
   */
  void open_input_port(unsigned int port_number = 0);

  /** @brief Closes the currently opened MIDI input device port.
   */
  void close_input_port();

  bool start() override { return false; }
  bool stop() override { return false; }

private:
  RtMidiIn m_rtmidi_in;
  std::shared_ptr<data::MidiCallbackContext> m_callback_context;
};

using MidiPortControllerPtr = std::shared_ptr<MidiPortController>;

} // namespace miniaudioengine::control

#endif // __MIDI_PORT_CONTROLLER_H__
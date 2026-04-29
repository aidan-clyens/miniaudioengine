#ifndef __MIDI_ADAPTER_H__
#define __MIDI_ADAPTER_H__

#include <rtmidi/RtMidi.h>
#include <string>
#include <vector>

#include "logger.h"

namespace miniaudioengine::adapters
{
  
/** @struct MidiPort
  * @brief Represents a MIDI port with its number and name.
  */
struct MidiPort
{
  unsigned int port_number;
  std::string port_name;
};

/** @class MidiCallbackHandler
 *  @brief Handles RtMidi callback functions.
 */
class MidiCallbackHandler
{
public:
  /** @brief Callback function to handle incoming MIDI messages.
   *  This function is called by the RtMidi library when a MIDI message is received.
   *  It prints the received MIDI message to the console.
   *
   *  @param deltatime The time in seconds since the last message was received.
   *  @param message A vector containing the MIDI message bytes.
   *  @param user_data A pointer to the MidiEngine object.
   */
  static void midi_callback(double deltatime, std::vector<unsigned char> *message, void *user_data) {}
};

class MidiAdapter
{
public:
  MidiAdapter() = default;
  MidiAdapter(const MidiAdapter&) = default;
  MidiAdapter& operator=(const MidiAdapter&) = default;
  virtual ~MidiAdapter() = default;

  unsigned int get_port_count()
  {
    return m_rtmidi_in.getPortCount();
  }

  std::vector<MidiPort> get_ports();

  bool open_input_port(unsigned int port_number, void *callback_context);
  bool close_input_port();

  bool is_port_open();

private:
  RtMidiIn m_rtmidi_in;
};

} // namespace miniaudioengine::adapters

#endif // __MIDI_ADAPTER_H__
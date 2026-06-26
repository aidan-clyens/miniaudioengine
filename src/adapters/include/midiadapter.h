#ifndef __MIDI_ADAPTER_H__
#define __MIDI_ADAPTER_H__

#include <string>
#include <vector>
#include <memory>
#include <rtmidi/RtMidi.h>

#include "device.h"
#include "logger.h"

namespace miniaudioengine::adapters
{

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
  static void midi_callback(double deltatime, std::vector<unsigned char> *message, void *user_data)
  {
    (void)deltatime;
    (void)user_data;
    (void)message;
  }
};

class MidiAdapter
{
public:
  MidiAdapter();
  MidiAdapter(const MidiAdapter&) = default;
  MidiAdapter& operator=(const MidiAdapter&) = default;
  virtual ~MidiAdapter() = default;

  unsigned int get_device_count();
  std::vector<DevicePtr> get_devices();

  bool open_input_port(DevicePtr device, void *callback_context);
  bool close_input_port();

  bool is_port_open();

private:
  std::unique_ptr<RtMidiIn> p_rtmidi_in;

  static DevicePtr make_device_handle(const unsigned int id, const std::string &name)
  {
    return DeviceHandleFactory::make_midi(
      id,
      name,
      true,   // TODO - How to get input/output info?
      false   // TODO - How to get input/output info?
    );
  }
};

using MidiAdapterPtr = std::shared_ptr<MidiAdapter>;

} // namespace miniaudioengine::adapters

#endif // __MIDI_ADAPTER_H__
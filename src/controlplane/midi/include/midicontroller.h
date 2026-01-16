#ifndef __MIDI_CONTROLLER_H__
#define __MIDI_CONTROLLER_H__

#include "controller.h"
#include "miditypes.h"

#include <vector>

namespace miniaudioengine::control
{

/** @class IMidiController
 *  @brief Interface for MIDI controllers in the framework.
 *  This class serves as a base for all MIDI-related controllers,
 *  providing a common interface and shared functionality.
 */
class IMidiController : public core::IController
{
public:
  virtual ~IMidiController() = default;

  virtual std::vector<MidiPort> get_ports() = 0;
  virtual void open_input_port(unsigned int port_number = 0) = 0;
  virtual void close_input_port() = 0;
};

} // namespace miniaudioengine::control

#endif // __MIDI_CONTROLLER_H__
#ifndef __MIDI_CONTROLLER_INTERFACE_H__
#define __MIDI_CONTROLLER_INTERFACE_H__

#include "controller.h"
#include "midicallbackhandler.h"
#include "miditypes.h"

#include <vector>

namespace miniaudioengine::midi
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

  virtual std::shared_ptr<core::MidiCallbackContext> get_callback_context() const
  {
    return m_callback_context;
  }

protected:
  IMidiController() :
    core::IController("IMidiController"),
    m_callback_context(std::make_shared<core::MidiCallbackContext>()) {}

  // Common state shared by all implementations
  std::shared_ptr<core::MidiCallbackContext> m_callback_context;
};

} // namespace miniaudioengine::midi

#endif // __MIDI_CONTROLLER_INTERFACE_H__

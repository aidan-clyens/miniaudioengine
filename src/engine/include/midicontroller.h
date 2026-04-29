#ifndef __MIDI_CONTROLLER_H__
#define __MIDI_CONTROLLER_H__

#include "interfaces/controller.h"
#include "mididataplane.h"
#include "miditypes.h"
#include "midiadapter.h"

#include <memory>
#include <vector>

namespace miniaudioengine::midi
{

/** @struct MidiCallbackContext
 *  @brief Context structure passed to the MIDI callback function.
 */
struct MidiCallbackContext
{
  std::vector<framework::MidiDataPlanePtr> active_tracks;
};

/** @class MidiController
 *  @brief This class manages the MIDI hardware interfaces.
 *  It is responsible for initializing, configuring, and controlling MIDI inputs.
 *  The MidiController provides methods to open and close MIDI input ports.
 *  @note This class is part of the control plane. Operations are synchronous and called from the main thread.
 */
class MidiController : public framework::IController
{
public:
  MidiController() : framework::IController("MidiController"),
                     m_callback_context(std::make_shared<MidiCallbackContext>()) {}

  ~MidiController() override
  {
    close_input_port();
  }

  /** @brief Gets the list of available MIDI input ports.
   *  @return A vector of MidiPort structures representing the available MIDI ports.
   */
  virtual std::vector<adapters::MidiPort> get_ports()
  {
    return m_midi_adapter.get_ports();
  }

  /** @brief Opens a MIDI input device port.
   *  @param port_number The MIDI device port number to open (default is 0).
   *  @throws std::out_of_range if the port number is invalid.
   *  @throws std::runtime_error if the port cannot be opened.
   */
  virtual void open_input_port(unsigned int port_number = 0);

  /** @brief Closes the currently opened MIDI input device port.
   */
  virtual void close_input_port();

  /** @brief Get the MIDI callback context shared with the data plane.
   *  @return Shared pointer to the MidiCallbackContext.
   */
  virtual std::shared_ptr<MidiCallbackContext> get_callback_context() const
  {
    return m_callback_context;
  }

protected:
  std::shared_ptr<MidiCallbackContext> m_callback_context;

private:
  adapters::MidiAdapter m_midi_adapter;

  bool _start() override { throw std::runtime_error("MidiController start/stop operations not implemented."); }
  bool _stop() override { throw std::runtime_error("MidiController start/stop operations not implemented."); }
};

using MidiControllerPtr = std::shared_ptr<MidiController>;

} // namespace miniaudioengine::midi

#endif // __MIDI_CONTROLLER_H__

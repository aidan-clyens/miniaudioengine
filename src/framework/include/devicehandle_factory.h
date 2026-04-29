#ifndef __DEVICE_HANDLE_FACTORY_H__
#define __DEVICE_HANDLE_FACTORY_H__

#include "device.h"

#include <string>
#include <vector>
#include <memory>

namespace miniaudioengine
{

/** @class DeviceHandleFactory
 *  @brief Internal factory for constructing Device objects.
 *  Not part of the public API. Only used within the library.
 *  Deliberately free of hardware-backend headers (RtAudio/RtMidi) so this
 *  header can be included by tests without pulling in hardware dependencies.
 */
class DeviceHandleFactory
{
public:
  /** @brief Create a Device for an audio device from explicit fields.
   *  @param id                   Hardware device ID.
   *  @param name                 Human-readable device name.
   *  @param is_default_input     True if system default input.
   *  @param is_default_output    True if system default output.
   *  @param output_channels      Number of output channels.
   *  @param input_channels       Number of input channels.
   *  @param duplex_channels      Number of duplex channels.
   *  @param preferred_sample_rate Preferred sample rate in Hz.
   *  @param sample_rates         Supported sample rates.
   *  @return Shared pointer to the constructed Device.
   */
  static DeviceHandlePtr make_audio(unsigned int id,
                                    const std::string& name,
                                    bool is_default_input,
                                    bool is_default_output,
                                    unsigned int output_channels,
                                    unsigned int input_channels,
                                    unsigned int duplex_channels,
                                    unsigned int preferred_sample_rate,
                                    const std::vector<unsigned int>& sample_rates);

  /** @brief Create a Device wrapping a MIDI port.
   *  @param id           Hardware port number.
   *  @param name         Human-readable port name.
   *  @param is_input     True if the port is an input port.
   *  @param is_output    True if the port is an output port.
   *  @return Shared pointer to the constructed Device.
   */
  static DeviceHandlePtr make_midi(unsigned int id,
                                   const std::string& name,
                                   bool is_input,
                                   bool is_output);
};

} // namespace miniaudioengine

#endif // __DEVICE_HANDLE_FACTORY_H__

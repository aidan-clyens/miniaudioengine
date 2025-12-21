#ifndef _MIDI_ENGINE_H
#define _MIDI_ENGINE_H

#include <memory>
#include <vector>

#include "miditypes.h"
#include "engine.h"
#include "subject.h"

class RtMidiIn;  // Forward declaration for RtMidiIn class

namespace MinimalAudioEngine
{

/** @class MidiEngine
 *  @brief The MidiEngine class is responsible for managing MIDI input.
 */
class MidiEngine : public IEngine<MidiMessage>, public Subject<MidiMessage>
{
public:
  static MidiEngine& instance()
  {
    static MidiEngine instance;
    return instance;
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

  void receive_midi_message(const MidiMessage& message) noexcept
  {
    push_message(message);
  }

private:
  MidiEngine();
  ~MidiEngine() override;

  void run() override
  {
    while (is_running())
    {
      handle_messages();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  void handle_messages() override 
  {
    while (auto message = pop_message())
    {
      if (!message.has_value())
        continue;
      notify(message.value());
    }
  }

  std::unique_ptr<RtMidiIn> p_midi_in;
};

}  // namespace MinimalAudioEngine

#endif  // _MIDI_ENGINE_H

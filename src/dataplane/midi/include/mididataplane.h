#ifndef __TRACK_MIDI_DATA_PLANE_H__
#define __TRACK_MIDI_DATA_PLANE_H__

#include <memory>
#include <atomic>
#include <string>

#include "dataplane.h"

namespace MinimalAudioEngine::Control
{
// Forward declaration
struct MidiMessage;
}

namespace MinimalAudioEngine::Data
{

/** @struct MidiInputStatistics
 *  @brief Statistics related to MIDI input processing.
 */
struct MidiInputStatistics
{
  size_t total_messages_processed = 0;

  MidiInputStatistics() = default;
  MidiInputStatistics(const MidiInputStatistics&) = default;
  MidiInputStatistics& operator=(const MidiInputStatistics&) = default;
  virtual ~MidiInputStatistics() = default;

  std::string to_string() const
  {
    return "MidiInputStatistics(\n  Total Messages Processed = " + std::to_string(total_messages_processed) + ")";
  }
};

/** @class MidiDataPlane
 *  @brief Data plane for handling MIDI messages for a track. The Data plane is only a callback
 *  target for RtMidi and is not a producer/consumer of MIDI messages itself.
 */
class MidiDataPlane : public Core::IDataPlane
{
public:
  /** @brief Process an incoming MIDI message. Called from the RtMidi callback function.
   *  @param midi_message The MIDI message to process.
   */  
  void process_midi_message(const Control::MidiMessage& midi_message);

  /** @brief Get MIDI input statistics.
   *  @return MidiInputStatistics structure containing input statistics.
   */
  MidiInputStatistics get_statistics() const
  {
    return m_midi_input_stats;
  }

private:
  MidiInputStatistics m_midi_input_stats;

private:
  void update_midi_input_statistics(const Control::MidiMessage& midi_message); 
};

typedef std::shared_ptr<MidiDataPlane> TrackMidiDataPlanePtr;

} // namespace MinimalAudioEngine::Data

#endif // __TRACK_MIDI_DATA_PLANE_H__
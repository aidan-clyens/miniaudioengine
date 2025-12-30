#ifndef __TRACK_MIDI_DATA_PLANE_H__
#define __TRACK_MIDI_DATA_PLANE_H__

#include "dataplane.h"
#include "lockfree_ringbuffer.h"

#include <memory>

namespace MinimalAudioEngine
{

// Forward declaration
struct MidiMessage;

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

/** @class TrackMidiDataPlane
 *  @brief Data plane for handling MIDI messages for a track. Implements IDataPlane.
 */
class TrackMidiDataPlane : public IDataPlane<MidiMessage, 1024>
{
public:
  void process_midi_message(const MidiMessage& midi_message);

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
  void update_midi_input_statistics(const MidiMessage& midi_message); 
};

typedef std::shared_ptr<TrackMidiDataPlane> TrackMidiDataPlanePtr;

} // namespace MinimalAudioEngine

#endif // __TRACK_MIDI_DATA_PLANE_H__